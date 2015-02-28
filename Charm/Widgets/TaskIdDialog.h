/*
  TaskIdDialog.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

#ifndef TASKIDDIALOG_H
#define TASKIDDIALOG_H

#include <QDialog>

#include "TasksView.h"
#include "Core/TaskModelInterface.h"
#include "ui_TaskIdDialog.h"

/**
 * Dialog shown when creating a task
 */
class TaskIdDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TaskIdDialog( TaskModelInterface* model, TasksView* parent );
    ~TaskIdDialog();

    void setSuggestedId( int );
    int selectedId() const;

    QString taskName() const;

private slots:
    void on_spinBox_valueChanged( int );

private:
    Ui::TaskIdDialog m_ui;
    TaskModelInterface* m_model;
};

#endif
