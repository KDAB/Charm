/*
  CharmCommand.cpp

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

#include "CharmCommand.h"
#include "CommandEmitterInterface.h"

#include <QMessageBox>

CharmCommand::CharmCommand( const QString& description, QObject *parent )
    : QObject( parent ),
      m_owner(0),
      m_description(description)
{
    CommandEmitterInterface* emitter = dynamic_cast<CommandEmitterInterface*>( parent );
    if ( emitter ) {
        m_owner = emitter;
    } else {
        Q_ASSERT_X( false, Q_FUNC_INFO,
                    "CharmCommand widget pointers have to implement the "
                    "CommandEmitterInterface." );
    }
}

CharmCommand::~CharmCommand()
{
}

QString CharmCommand::description() const
{
    return m_description;
}

CommandEmitterInterface* CharmCommand::owner() const
{
    return m_owner;
}

void CharmCommand::requestExecute()
{
    emit emitExecute(this);
}

void CharmCommand::requestRollback()
{
    emit emitRollback(this);
}

void CharmCommand::requestSlotEventIdChanged(int oldId, int newId)
{
    emit emitSlotEventIdChanged(oldId,newId);
}

void CharmCommand::showInformation(const QString& title, const QString& message)
{
    QWidget* parent = dynamic_cast<QWidget*>( owner() );
    Q_ASSERT( parent );
    QMessageBox::information( parent, title, message );
}

void CharmCommand::showCritical(const QString& title, const QString& message)
{
    QWidget* parent = dynamic_cast<QWidget*>( owner() );
    Q_ASSERT( parent );
    QMessageBox::critical( parent, title, message );
}

#include "moc_CharmCommand.cpp"
