/*
  CommandAddTask.cpp

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

#include "CommandAddTask.h"

#include "Core/ControllerInterface.h"
#include "Core/CommandEmitterInterface.h"

CommandAddTask::CommandAddTask( const Task& task, QObject* parent )
    : CharmCommand( tr("Add Task"), parent )
    , m_task( task )
    , m_success( false )
{
}

CommandAddTask::~CommandAddTask()
{
}

bool CommandAddTask::prepare()
{
    return true;
}

bool CommandAddTask::execute( ControllerInterface* controller )
{
    m_success = controller->addTask( m_task );
    return m_success;
}

bool CommandAddTask::finalize()
{
    if ( !m_success ) {
        showInformation( tr( "Unable to add task" ), tr( "Adding the task failed." ) );
    }
    return m_success;
}

#include "moc_CommandAddTask.cpp"
