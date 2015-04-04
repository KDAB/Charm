/*
  EnterVacationDialog.h

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

#ifndef ENTERVACATIONDIALOG_H
#define ENTERVACATIONDIALOG_H

#include <QDialog>

#include "Core/Event.h"
#include "Core/Task.h"

#include <QScopedPointer>

namespace Ui {
    class EnterVacationDialog;
}

class EnterVacationDialog : public QDialog {
    Q_OBJECT
public:
    explicit EnterVacationDialog( QWidget* parent=nullptr );
    ~EnterVacationDialog();

    EventList events() const;

private:
    void updateTaskLabel();
    void createEvents();

private slots:
    void selectTask();
    void okClicked();
    void updateButtonStates();

private:
    QScopedPointer<Ui::EnterVacationDialog> m_ui;
    TaskId m_selectedTaskId;
    EventList m_events;
};

#endif
