#include <iostream>

#include <QFile>
#include <QMessageBox>
#include <QtPlugin>
#include <QSettings>
#include <QString>

#include "Application.h"
#include "Core/CharmExceptions.h"

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

    Application app ( argc,  argv );

    try {
        return app.exec();
    } catch( CharmException& e ) {
        const QString msg( QObject::tr( "An application exception has occured. Charm will be terminated. The error message was:\n"
                                       "%1\n"
                                       "Please report this as a bug at https://quality.kdab.com/browse/CHM." ).arg( e.what() ) );
        QMessageBox::critical( 0, QObject::tr( "Application Error" ), msg );
        using namespace std;
        cerr << qPrintable( msg ) << endl;
    }
}
