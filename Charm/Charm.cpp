/*
  Charm.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2007-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>
  Author: Mike McQuaid <mike.mcquaid@kdab.com>
  Author: Frank Osterfeld <frank.osterfeld@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <memory>

#include <QApplication>
#include <QFile>
#include <QMessageBox>
#include <QSettings>
#include <QString>

#include "ApplicationCore.h"
#include "MacApplicationCore.h"
#include "Core/CharmExceptions.h"
#include "CharmCMake.h"

static std::shared_ptr<ApplicationCore> createApplicationCore()
{
#ifdef Q_OS_OSX
    return std::make_shared<MacApplicationCore>();
#else
    return std::make_shared<ApplicationCore>();
#endif
}

void showCriticalError( const QString& msg ) {
    QMessageBox::critical( nullptr, QObject::tr( "Application Error" ), msg );
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
        QApplication app( argc, argv );
        const std::shared_ptr<ApplicationCore> core( createApplicationCore() );
        QObject::connect( &app, SIGNAL(commitDataRequest(QSessionManager&)), core.get(), SLOT(commitData(QSessionManager&)) );
        QObject::connect( &app, SIGNAL(saveStateRequest(QSessionManager&)), core.get(), SLOT(saveState(QSessionManager&)) );
        return app.exec();
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
