/*
  CommandMakeEvent.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2007-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>
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

#include "CommandMakeEvent.h"
#include "Core/ControllerInterface.h"
#include "Widgets/EventView.h"

#include <QDateTime>

CommandMakeEvent::CommandMakeEvent( const Task& task,
                                    QObject* parent )
    : CharmCommand( tr("Create Event"), parent )
    , m_rollback( false )
    , m_task( task )
{
}

CommandMakeEvent::CommandMakeEvent( const Event& event,
                                    QObject* parent )
    : CharmCommand( tr("Create Event"), parent )
    , m_rollback( false )
    , m_event( event )
{
}



CommandMakeEvent::~CommandMakeEvent()
{
}

bool CommandMakeEvent::prepare()
{
    return true;
}

bool CommandMakeEvent::execute( ControllerInterface* controller )
{
    m_rollback = false;

    if(m_event.id()) //if it already has an id, this is a redo operation
    {
        int oid = m_event.id();
        m_event = controller->cloneEvent(m_event);
        int nid = m_event.id();
        if(oid != nid)
            emit emitSlotEventIdChanged(oid, nid);
        return m_event.isValid();
    }

    Event event = controller->makeEvent( m_task );
    if ( !event.isValid() )
         return false;

    QDateTime start( QDateTime::currentDateTime() );
    event.setStartDateTime( start );
    event.setEndDateTime( start );

    if ( m_event.startDateTime().isValid() )
        event.setStartDateTime( m_event.startDateTime() );
    if ( m_event.endDateTime().isValid() )
        event.setEndDateTime( m_event.endDateTime() );
    if ( !m_event.comment().isEmpty() )
        event.setComment( m_event.comment() );
    if ( m_event.taskId() != 0 )
        event.setTaskId( m_event.taskId() );

    if ( controller->modifyEvent( event ) ) {
        m_event = event;
        return true;
    } else {
        return false;
    }
}

bool CommandMakeEvent::rollback(ControllerInterface *controller )
{
    m_rollback = true;
    return controller->deleteEvent(m_event);
}

bool CommandMakeEvent::finalize()
{
    if ( m_rollback )
        return false;
    if ( m_event.isValid() ) {
        EventView* view = dynamic_cast<EventView*>( owner() );
        if ( view )
            view->makeVisibleAndCurrent( m_event );
        emit finishedOk( m_event );
        return true;
    } else {
        return false;
    }
}

void CommandMakeEvent::eventIdChanged(int oid, int nid)
{
    if(m_event.id() == oid)
        m_event.setId(nid);
}

#include "moc_CommandMakeEvent.cpp"
