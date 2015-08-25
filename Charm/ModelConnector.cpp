/*
  ModelConnector.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2007-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>
  Author: Frank Osterfeld <frank.osterfeld@kdab.com>
  Author: David Faure <david.faure@kdab.com>
  Author: Mike McQuaid <mike.mcquaid@kdab.com>
  Author: Allen Winter <allen.winter@kdab.com>

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

#include "ModelConnector.h"
#include "ViewHelpers.h"
#include "Data.h"

#include "Commands/CommandModifyEvent.h"
#include "Commands/CommandMakeAndActivateEvent.h"

ModelConnector::ModelConnector()
    : QObject()
    , m_dataModel()
    , m_viewFilter( &m_dataModel )
    , m_eventModelFilter( &m_dataModel )
    , m_findEventModelFilter( &m_dataModel )
{
    connect( &m_dataModel, SIGNAL(makeAndActivateEvent(Task)),
             SLOT(slotMakeAndActivateEvent(Task)) );
    connect( &m_dataModel, SIGNAL(requestEventModification(Event,Event)),
             SLOT(slotRequestEventModification(Event,Event)) );
    connect( &m_dataModel, SIGNAL(sysTrayUpdate(QString,bool)),
             SLOT(slotSysTrayUpdate(QString,bool)) );
}

CharmDataModel* ModelConnector::charmDataModel()
{
    return &m_dataModel;
}

ViewFilter* ModelConnector::taskModel()
{
    return &m_viewFilter;
}

EventModelFilter* ModelConnector::eventModel()
{
    return &m_eventModelFilter;
}

EventModelFilter* ModelConnector::findEventModel()
{
    return &m_findEventModelFilter;
}

void ModelConnector::commitCommand( CharmCommand* command )
{
    if ( ! command->finalize() ) {
        qDebug() << "CharmDataModel::commitCommand:"
                 << command->metaObject()->className()
                 << "command has failed";
    }
}

void ModelConnector::slotMakeAndActivateEvent( const Task& task )
{
    // the command will call activateEvent in finalize, this will
    // notify the task view to update
    auto command = new CommandMakeAndActivateEvent( task, this );
    VIEW.sendCommand( command );
}

void ModelConnector::slotRequestEventModification( const Event& newEvent, const Event& oldEvent )
{
    auto command = new CommandModifyEvent( newEvent, oldEvent, this );
    VIEW.sendCommand( command );
}

void ModelConnector::slotSysTrayUpdate(const QString& toolTip, bool active)
{
    TRAY.setToolTip( toolTip );
    TRAY.setIcon( active ? Data::charmTrayActiveIcon() : Data::charmTrayIcon() );
}

#include "moc_ModelConnector.cpp"
