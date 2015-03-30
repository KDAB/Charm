/*
  CommandRelayCommand.cpp

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

#include "CommandRelayCommand.h"
#include "Core/CommandEmitterInterface.h"

#include <QApplication>

CommandRelayCommand::CommandRelayCommand( QObject* parent )
    : CharmCommand( tr("Relay"), parent )
    , m_payload( 0 )
{   // as long as Charm is single-threaded, this does not do anything,
    // because there will be no repaint
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );
}

CommandRelayCommand::~CommandRelayCommand()
{
    QApplication::restoreOverrideCursor();
}

void CommandRelayCommand::setCommand( CharmCommand* command )
{
    m_payload = command;
}

bool CommandRelayCommand::prepare()
{
    Q_ASSERT_X( false, Q_FUNC_INFO,
                "Prepare should have been called by the owner instead." );
    return true;
}

bool CommandRelayCommand::execute( ControllerInterface* controller )
{
    return m_payload->execute( controller );
}

bool CommandRelayCommand::rollback( ControllerInterface* controller )
{
    return m_payload->rollback( controller );
}

bool CommandRelayCommand::finalize()
{
    QApplication::restoreOverrideCursor();
    m_payload->owner()->commitCommand( m_payload );
    return true;
}

#include "moc_CommandRelayCommand.cpp"
