#include <QDir>
#include <QtSql>
#include <QtDebug>
#include <QFileInfo>
#include <QSettings>
#include <QSqlDatabase>

// FIXME move dump<shit> to SqlStorage
#include "Event.h"
#include "Configuration.h"
#include "SqLiteStorage.h"

const char DatabaseName[] = "charm.kdab.net";

SqLiteStorage::SqLiteStorage()
    : SqlStorage()
    , m_database( QSqlDatabase::addDatabase( "QSQLITE", DatabaseName ) )
{
}

SqLiteStorage::~SqLiteStorage()
{
}

QString SqLiteStorage::description() const
{
    return QObject::tr( "local database" );
}

// FIXME Application should adjust the path, storage should simply
// accept it (rationale: no app logic in the storage)
bool SqLiteStorage::connect( Configuration& configuration )
{   // make sure the database folder exits:
    m_installationId = configuration.installationId;
    bool error = false;

    QFileInfo fileInfo( configuration.localStorageDatabase ); // this is the full path

    // make sure the path exists, file will be created by sqlite
    if ( QDir( fileInfo.absolutePath() ).exists() )
    {
//         qDebug() << "SqLiteStorage::configure: data folder exists at"
//                  << fileInfo.absolutePath();
    } else {
        if ( ! QDir().mkpath( fileInfo.absolutePath() ) )
            Q_ASSERT_X( false, "SqLiteStorage::configure",
                        "Cannot make database directory" );
//         qDebug() << "SqLiteStorage::configure: data folder created";
    }

    Q_ASSERT_X( QDir( fileInfo.absolutePath() ).exists(), "SqLiteStorage::connect",
                "I made a directory, but it is not there. Weird." );

    // connect:
//     qDebug() << "SqLiteStorage::connect: creating or opening local sqlite database at "
//              << fileInfo.absoluteFilePath();

    m_database.setHostName( "localhost" );
    m_database.setDatabaseName( fileInfo.absoluteFilePath() );

    if ( ! fileInfo.exists() && ! configuration.newDatabase ) {
        error = true;
        configuration.failureMessage = QObject::tr(
            "<html><head><meta name=\"qrichtext\" content=\"1\" /></head>"
            "<body><p>The configuration seems to be valid, but the database "
            "file does not exist.</p>"
            "<p>The file will automatically be generated. Please verify "
            "the configuration.</p>"
            "<p>If the configuration is correct, just close the dialog.</p>"
            "</body></html>");
    }

    bool ok = m_database.open();
    if ( ok ) {
//         qDebug() << "SqLiteStorage::connect: SUCCESS - connected to database";
        if ( verifyDatabase() )
        {
//             qDebug() << "SqLiteStorage::connect: database seems to be prepared";
        } else {
//             qDebug() << "SqLiteStorage::connect: empty database, filling in the blanks";
            if ( !createDatabase( configuration ) ) {
                qDebug() << "SqLiteStorage::connect: error creating default database contents";
                return false;
            }

        }
        int userid = configuration.user.id();
        User user = getUser( userid );
//         qDebug() << "SqLiteStorage::connect: found user" << user.name()
//                  << "for id" << userid << ( user.isValid() ? "(valid)" : "(invalid)");
        if ( user.isValid() ) {
            configuration.user = user;
        } else {
            return false;
        }
    } else {
        qDebug() << "SqLiteStorage::connect: FAILURE - cannot connect to database";
        return false;
    }

    if ( ! ok || error ) {
        configuration.failure = true;
        return false;
    } else {
        return true;
    }
}

bool SqLiteStorage::disconnect()
{
    m_database.removeDatabase( DatabaseName );
    m_database.close();
    return true; // neither of the two methods return a value
}

int SqLiteStorage::installationId() const
{
    return m_installationId;
}

QSqlDatabase& SqLiteStorage::database()
{
    return m_database;
}

bool SqLiteStorage::createDatabase( Configuration& configuration )
{
    bool success = SqlStorage::createDatabase();
    if ( !success ) return false;

    // add installation id and user id:
    const QString userName = configuration.user.name();
    configuration.user = makeUser( userName );
    if ( ! configuration.user.isValid() ) {
        qDebug() << "SqLiteStorage::createDatabase: cannot store user name";
        return false;
    }

    // make an installation:
    // FIXME make a useful name for it
    QString installationName = "Unnamed Installation";
    Installation installation = createInstallation( installationName );
    if ( ! installation.isValid() ) {
        qDebug() << "SqLiteStorage::createDatabase: cannot create default installation id";
        return false;
    } else {
        configuration.installationId = installation.id();
    }

    return true;
}
