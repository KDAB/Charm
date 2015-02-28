/*
  TrayIcon.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

#include "TrayIcon.h"

#include "ApplicationCore.h"


TrayIcon::TrayIcon(QObject* parent)
{
    connect(this,
            SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            SLOT(slotActivated(QSystemTrayIcon::ActivationReason)));
}

TrayIcon::~TrayIcon()
{

}

void TrayIcon::slotActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
    case QSystemTrayIcon::Context:
        // show context menu
        // m_systrayContextMenu.show();
        break;
    case QSystemTrayIcon::Trigger: //(single click)
    case QSystemTrayIcon::DoubleClick:
#ifndef Q_OS_OSX
        ApplicationCore::instance().toggleShowHide();
#endif
        break;
    case QSystemTrayIcon::MiddleClick:
        // TODO: Start task?
        ApplicationCore::instance().slotStopAllTasks();
        break;
    case QSystemTrayIcon::Unknown:
    default:
        break;
    }
}

#include "moc_TrayIcon.cpp"
