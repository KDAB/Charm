/*
  TaskModelInterface.h

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

#ifndef MODELINTERFACE_H
#define MODELINTERFACE_H

class QModelIndex;

#include "Task.h"
#include "Event.h"

class TaskModelInterface
{
public:
    virtual ~TaskModelInterface() {}
    virtual Task taskForIndex( const QModelIndex& ) const = 0;
    virtual QModelIndex indexForTaskId( TaskId ) const = 0;
    virtual bool taskIsActive( const Task& task ) const = 0;
    virtual bool taskHasChildren( const Task& task ) const = 0;
    virtual bool taskIdExists( TaskId taskId ) const = 0;
    // relayed model signals, in lack of notification in the view:
    // eventActivated was already taken by CharmDataModelAdapterInterface
    virtual void eventActivationNotice( EventId id ) = 0;
    virtual void eventDeactivationNotice( EventId id ) = 0;
};

#endif
