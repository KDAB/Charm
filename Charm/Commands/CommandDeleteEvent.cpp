/*
  CommandDeleteEvent.cpp

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

#include "CommandDeleteEvent.h"
#include "Core/ControllerInterface.h"

CommandDeleteEvent::CommandDeleteEvent( const Event& event, QObject* parent )
    : CharmCommand( tr("Delete Event"), parent )
    , m_event( event )
{
}

CommandDeleteEvent::~CommandDeleteEvent()
{
}

bool CommandDeleteEvent::prepare()
{
    return true;
}

bool CommandDeleteEvent::execute( ControllerInterface* controller )
{
    qDebug() << "CommandDeleteEvent::execute: deleting:";
    m_event.dump();
    return controller->deleteEvent( m_event );
}

bool CommandDeleteEvent::rollback(ControllerInterface *controller)
{
    int oldId = m_event.id();
    m_event = controller->cloneEvent(m_event);
    int newId = m_event.id();
    if(oldId != newId)
        emit emitSlotEventIdChanged(oldId, newId);
    return m_event.isValid();
}

bool CommandDeleteEvent::finalize()
{
    return true;
}

void CommandDeleteEvent::eventIdChanged(int oid, int nid)
{
    if(m_event.id() == oid)
        m_event.setId(nid);
}

#include "moc_CommandDeleteEvent.cpp"
