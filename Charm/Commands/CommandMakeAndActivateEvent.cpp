/*
  CommandMakeAndActivateEvent.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2007-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>

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

#include "CommandMakeAndActivateEvent.h"
#include "ModelConnector.h"

#include "Core/CharmDataModel.h"
#include "Core/CommandEmitterInterface.h"
#include "Core/ControllerInterface.h"

#include <QDateTime>

CommandMakeAndActivateEvent::CommandMakeAndActivateEvent( const Task& task,
                                                          QObject* parent )
    : CharmCommand( tr("Create Event"), parent )
    , m_task( task )
{
}

CommandMakeAndActivateEvent::~CommandMakeAndActivateEvent()
{
}

bool CommandMakeAndActivateEvent::prepare()
{
    return true;
}

bool CommandMakeAndActivateEvent::execute( ControllerInterface* controller )
{
    m_event = controller->makeEvent( m_task );
    if ( m_event.isValid() ) {
        m_event.setTaskId( m_task.id() );
        m_event.setStartDateTime( QDateTime::currentDateTime() );
        return controller->modifyEvent( m_event );
    } else {
        return false;
    }
}

bool CommandMakeAndActivateEvent::finalize()
{
    if ( m_event.isValid() ) {
        ModelConnector* model = dynamic_cast<ModelConnector*>( owner() );
        Q_ASSERT( model ); // this command is "owned" by the model
        model->charmDataModel()->activateEvent( m_event );
        return true;
    } else {
        return false;
    }
}

#include "moc_CommandMakeAndActivateEvent.cpp"
