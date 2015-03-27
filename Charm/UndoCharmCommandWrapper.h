/*
  UndoCharmCommandWrapper.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2012-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Nicholas Van Sickle <nicholas.vansickle@kdab.com>

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

#ifndef UNDOCHARMCOMMANDWRAPPER_H
#define UNDOCHARMCOMMANDWRAPPER_H

#include <QUndoCommand>

#include "Core/CharmCommand.h"

/**
  Thin wrapper for CharmCommand -> QUndoCommand

  It simply forwards the command text and emits signals for commit/rollback on undo/redo
  **/
class UndoCharmCommandWrapper : public QUndoCommand
{
public:
    explicit UndoCharmCommandWrapper(CharmCommand* command);
    ~UndoCharmCommandWrapper();
    void undo() override;
    void redo() override;
    CharmCommand* command() const;
private:
    CharmCommand* m_command;
};

#endif
