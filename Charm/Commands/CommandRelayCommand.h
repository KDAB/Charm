/*
  CommandRelayCommand.h

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

#ifndef COMMANDRELAYCOMMAND_H
#define COMMANDRELAYCOMMAND_H

#include <Core/CharmCommand.h>

/** CommandRelayCommand is a decorator class that is used to wrap all
    commands send by the view.
    ATM, CommandRelayCommand sets the hour glass cursor on the view
    and resets it when it is deleted.
*/
class CommandRelayCommand : public CharmCommand
{
    Q_OBJECT

public:
    explicit CommandRelayCommand( QObject* parent );
    ~CommandRelayCommand();

    void setCommand( CharmCommand* command );

    bool prepare() override;
    bool execute( ControllerInterface* ) override;
    bool rollback( ControllerInterface* ) override;
    bool finalize() override;

private:
    CharmCommand* m_payload;
};

#endif
