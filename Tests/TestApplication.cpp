#include <QString>
#include <QFileInfo>
#include <QtDebug>
#include <QDir>
#include <QtTest/QtTest>

#include "Core/Configuration.h"
#include "Core/CharmConstants.h"
#include "Core/Controller.h"
#include "Core/Configuration.h"
#include "Core/CharmDataModel.h"
#include "TestApplication.h"

const int UserId = 1;
const int InstallationId = 1;

TestApplication::TestApplication(const QString &databasePath, QObject *parent)
    : QObject(parent)
    , m_controller( 0 )
    , m_model( 0 )
    , m_configuration( &Configuration::instance() )
    , m_localPath( databasePath )
{
}

Controller* TestApplication::controller()
{
    return m_controller;
}

CharmDataModel* TestApplication::model()
{
    return m_model;
}

Configuration* TestApplication::configuration()
{
    return m_configuration;
}

QString TestApplication::databasePath()
{
    return m_localPath;
}

void TestApplication::initialize()
{
    QFileInfo file( m_localPath );
    if ( file.exists() ) {
        qDebug() << "test database file exists, deleting";
        QDir dir( file.absoluteDir() );
        QVERIFY( dir.remove( file.fileName() ) );
    }

    // well, here it gets a bit more challenging - this is not for
    // sissies:
    // - make a controller
    // - make it create a local storage backend
    // - make a data model and connect it to the controller
    // - stimulate the controller and see if the right content ends up
    //   in the database and the model
    // -----
    // ... make the controller:
    m_configuration->installationId = InstallationId;
    m_configuration->user.setId( UserId );
    m_configuration->localStorageType = CHARM_SQLITE_BACKEND_DESCRIPTOR;
    m_configuration->localStorageDatabase = m_localPath;
    m_configuration->newDatabase = true;
    m_controller = new Controller;
    // ... initialize the backend:
    QVERIFY( m_controller->initializeBackEnd( CHARM_SQLITE_BACKEND_DESCRIPTOR ) );
    QVERIFY( m_controller->connectToBackend() );
    // ... make the data model:
    m_model = new CharmDataModel;
    // ... connect model and controller:
    connectControllerAndModel( m_controller, m_model );
    QVERIFY( m_controller->storage() != 0 );
}

void TestApplication::destroy()
{
    QVERIFY( controller()->disconnectFromBackend() );
    delete m_model; m_model = 0;
    delete m_controller; m_controller = 0;
    if ( QDir::home().exists( databasePath() ) ) {
        const bool result = QDir::home().remove( databasePath() );
        QVERIFY( result );
    }
}

int TestApplication::testUserId()
{
    return 1;
}

int TestApplication::testInstallationId()
{
    return 1;
}

#include "moc_TestApplication.cpp"
