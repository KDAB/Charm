#include <iostream>

#include <QApplication>
#include <QFile>
#include <QMessageBox>
#include <QtPlugin>
#include <QSettings>
#include <QString>

#include "ApplicationFactory.h"
#include "Core/CharmExceptions.h"
#include "CharmCMake.h"

void showCriticalError( const QString& msg ) {
    QMessageBox::critical( 0, QObject::tr( "Application Error" ), msg );
    using namespace std;
    cerr << qPrintable( msg ) << endl;
}

int main ( int argc, char** argv )
{
    if (argc == 2 && qstrcmp(argv[1], "--version") == 0) {
        using namespace std;
        cout << "Charm version " << CHARM_VERSION << endl;
        return 0;
    }

    const QByteArray charmHomeEnv = qgetenv("CHARM_HOME");
    if ( !charmHomeEnv.isEmpty() ) {
        const QString charmHome = QFile::decodeName( charmHomeEnv );
        const QString user = charmHome + QLatin1String("/userConfig");
        const QString sys = charmHome + QLatin1String("/systemConfig");
        QSettings::setPath( QSettings::NativeFormat, QSettings::UserScope, user );
        QSettings::setPath( QSettings::IniFormat, QSettings::UserScope, user );
        QSettings::setPath( QSettings::NativeFormat, QSettings::SystemScope, sys );
        QSettings::setPath( QSettings::IniFormat, QSettings::SystemScope, sys );
    }

    try {
        QApplication *app = ApplicationFactory::localApplication( argc, argv );
        return app->exec();
    } catch( const AlreadyRunningException& ) {
        using namespace std;
        cout << "Charm already running, exiting..." << endl;
        return 0;
    } catch( const CharmException& e ) {
        const QString msg( QObject::tr( "An application exception has occurred. Charm will be terminated. The error message was:\n"
                                       "%1\n"
                                       "Please report this as a bug at https://quality.kdab.com/browse/CHM." ).arg( e.what() ) );
        showCriticalError( msg );
        return 1;
    } catch( ... ) {
        const QString msg( QObject::tr( "The application terminated with an unexpected exception.\n"
                                       "No other information is available to debug this problem.\n"
                                       "Please report this as a bug at https://quality.kdab.com/browse/CHM." ) );
        showCriticalError( msg );
        return 1;
    }
    return 0;
}
