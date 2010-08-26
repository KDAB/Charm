#include <QFile>
#include <QMessageBox>
#include <QtPlugin>
#include <QSettings>
#include <QString>

#include "Application.h"

int main ( int argc, char** argv )
{
    if (argc == 2 && qstrcmp(argv[1], "--version") == 0) {
        printf("Charm version " CHARM_VERSION "\n");
        return 0;
    }

    if ( const char* charmHomeEnv = qgetenv("CHARM_HOME") ) {
        const QString charmHome = QFile::decodeName( charmHomeEnv );
        const QString user = charmHome + QLatin1String("/userConfig");
        const QString sys = charmHome + QLatin1String("/systemConfig");
        QSettings::setPath( QSettings::NativeFormat, QSettings::UserScope, user );
        QSettings::setPath( QSettings::IniFormat, QSettings::UserScope, user );
        QSettings::setPath( QSettings::NativeFormat, QSettings::SystemScope, sys );
        QSettings::setPath( QSettings::IniFormat, QSettings::SystemScope, sys );
    }

    Application app ( argc,  argv );
    return app.exec();
}
