/*
  CharmDataModelAdapterInterface.h

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

#ifndef CHARMDATAMODELADAPTERINTERFACE_H
#define CHARMDATAMODELADAPTERINTERFACE_H

#include "Task.h"
#include "Event.h"
#include <QList>

class CharmDataModelAdapterInterface
{
public:
    // keep compiler happy:
    virtual ~CharmDataModelAdapterInterface() {}

    virtual void resetTasks() = 0;
    virtual void taskAboutToBeAdded( TaskId parent, int pos ) = 0;
    virtual void taskAdded( TaskId id ) = 0;
    virtual void taskModified( TaskId id ) = 0;
    virtual void taskParentChanged( TaskId task, TaskId oldParent, TaskId newParent ) = 0;
    virtual void taskAboutToBeDeleted( TaskId ) = 0;
    virtual void taskDeleted( TaskId id ) = 0;

    virtual void resetEvents() = 0;
    virtual void eventAboutToBeAdded( EventId id ) = 0;
    virtual void eventAdded( EventId id ) = 0;
    // we only pass an event because it is an outdated object:
    virtual void eventModified( EventId id, Event discardedEvent ) = 0;
    virtual void eventAboutToBeDeleted( EventId id ) = 0;
    virtual void eventDeleted( EventId id ) = 0;

    virtual void eventActivated( EventId id ) = 0;
    virtual void eventDeactivated( EventId id ) = 0;
};

typedef QList<CharmDataModelAdapterInterface*> CharmDataModelAdapterList;

#endif
