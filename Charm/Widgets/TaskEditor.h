/*
  TaskEditor.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2008-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

#ifndef TASKEDITOR_H
#define TASKEDITOR_H

#include <QDialog>

#include "Core/Task.h"

#include <QScopedPointer>

namespace Ui {
    class TaskEditor;
}

class TaskEditor: public QDialog
{
    Q_OBJECT

public:
    explicit TaskEditor( QWidget* parent = nullptr );
    virtual ~TaskEditor();

    void setTask( const Task& task );

    Task getTask() const;

private Q_SLOTS:
    void slotSelectParent();
    void slotDateChanged( const QDate & date );
    void slotCheckBoxChecked( bool );

private:
    void checkInvariants();

    QScopedPointer<Ui::TaskEditor> m_ui;
    Task m_task;
};

#endif /* TASKEDITOR_H */
