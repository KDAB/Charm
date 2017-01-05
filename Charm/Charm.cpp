/*
  Charm.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2007-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

static std::shared_ptr<ApplicationCore> createApplicationCore( TaskId startupTask )
{
#ifdef Q_OS_OSX
    return std::make_shared<MacApplicationCore>( startupTask );
#else
    return std::make_shared<ApplicationCore>( startupTask );
#endif
}

void showCriticalError( const QString& msg ) {
    QMessageBox::critical( nullptr, QObject::tr( "Application Error" ), msg );
    using namespace std;
    cerr << qPrintable( msg ) << endl;
}

int main ( int argc, char** argv )
{
    TaskId startupTask = -1;
    if (argc >= 2) {
        if ( qstrcmp(argv[1], "--version") == 0) {
            using namespace std;
            cout << "Charm version " << CHARM_VERSION << endl;
            return 0;
        } else if ( argc == 3 && qstrcmp(argv[1], "--start-task") == 0 ) {
            bool ok = true;
            startupTask = QString::fromLocal8Bit( argv[2] ).toInt( &ok );
            if ( !ok || startupTask < 0 ) {
                std::cerr << "Invalid task id passed: " << argv[2];
                return 1;
            }
        }
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
#ifdef Q_OS_WIN
        // Use ini for storing settings as the registry path is not affected by CHARM_HOME.
        QSettings::setDefaultFormat( QSettings::IniFormat );
#endif
    }

    try {
        // High DPI support
#if QT_VERSION >= QT_VERSION_CHECK(5,1,0)
        QGuiApplication::setAttribute( Qt::AA_UseHighDpiPixmaps, true );
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5,6,0)
        // High DPI support
        QGuiApplication::setAttribute( Qt::AA_EnableHighDpiScaling );
        QGuiApplication::setAttribute( Qt::AA_UseHighDpiPixmaps, true );
#endif
        QApplication app( argc, argv );
        const std::shared_ptr<ApplicationCore> core( createApplicationCore( startupTask ) );
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
