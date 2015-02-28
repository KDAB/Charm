/*
  WindowsIdleDetector.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Frank Osterfeld <frank.osterfeld@kdab.com>
  Author: Mike McQuaid <mike.mcquaid@kdab.com>

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

#include "WindowsIdleDetector.h"

#include "windows.h"

#include <QDebug>

WindowsIdleDetector::WindowsIdleDetector( QObject* parent ) : IdleDetector( parent ) {
    connect( &m_timer, SIGNAL(timeout()), this, SLOT(timeout()) );
    m_timer.setInterval( idlenessDuration() * 1000 / 2 );
    m_timer.setSingleShot( false );
    m_timer.start();
}

void WindowsIdleDetector::onIdlenessDurationChanged() {
    m_timer.stop();
    m_timer.setInterval( idlenessDuration() * 1000 / 2 );
    m_timer.start();
}

void WindowsIdleDetector::timeout() {
    LASTINPUTINFO lif;
    lif.cbSize = sizeof( lif );
    const bool ret = GetLastInputInfo( &lif );
    if ( !ret ) {
        qWarning() << "Idle detection: GetLastInputInfo failed.";
        return;
    }

    const qint64 dwTime = static_cast<qint64>( lif.dwTime );
    const qint64 ctk = static_cast<qint64>( GetTickCount() );
    const int idleSecs = ( ctk - dwTime ) / 1000;
    if ( idleSecs >= idlenessDuration() )
        maybeIdle( IdlePeriod(QDateTime::currentDateTime().addSecs( -idleSecs ),
                              QDateTime::currentDateTime() ) );
}

#include "moc_WindowsIdleDetector.cpp"
