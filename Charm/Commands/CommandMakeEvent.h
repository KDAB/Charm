/*
  CommandMakeEvent.h

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

#ifndef COMMANDMAKEEVENT_H
#define COMMANDMAKEEVENT_H

#include <Core/Task.h>
#include <Core/Event.h>
#include <Core/CharmCommand.h>

class QObject;

class CommandMakeEvent : public CharmCommand
{
    Q_OBJECT

public:
    explicit CommandMakeEvent( const Task& task, QObject* parent );
    explicit CommandMakeEvent( const Event& event, QObject* parent );
    ~CommandMakeEvent();

    bool prepare() override;
    bool execute( ControllerInterface* ) override;
    bool rollback( ControllerInterface* ) override;
    bool finalize() override;

public slots:
    void eventIdChanged(int,int) override;

Q_SIGNALS:
    void finishedOk( const Event& );

private:
    bool m_rollback; //don't show the event in finalize
    Task m_task; // the task the new event should be assigned to
    Event m_event; // the result, only valid after the event has been created
};

#endif
