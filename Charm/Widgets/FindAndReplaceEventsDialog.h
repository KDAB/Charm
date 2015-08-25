/*
  FindAndReplaceEventsDialog.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Michel Boyer de la Giroday <michel.gitroday@kdab.com>

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


#ifndef FINDANDREPLACEEVENTSDIALOG_H
#define FINDANDREPLACEEVENTSDIALOG_H

#include <QDialog>
#include <QScopedPointer>

#include "Core/Task.h"
#include <Core/TimeSpans.h>

class Event;
class EventModelFilter;
class Task;

namespace Ui {
class FindAndReplaceEventsDialog;
}

class FindAndReplaceEventsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindAndReplaceEventsDialog( QWidget* parent = nullptr );
    ~FindAndReplaceEventsDialog();

    QList<Event> modifiedEvents() const;

private slots:
    void slotSelectTaskToSearch();
    void slotSelectTaskToReplaceWith();
    void slotTimeSpansChanged();
    void slotReplaceProjectCode();

private:

    enum SelectTaskType {
         TaskToSearch,
         TaskToReplaceWith
    };

    void searchProjectCode();
    void selectTask( SelectTaskType type );
    void eventChangesCompleted( const Event& event );

    TaskId m_taskToSearch;
    TaskId m_taskToReplaceWith;
    TimeSpan m_timeSpan;
    QPushButton* m_replace;
    QPushButton* m_cancel;
    QList<Event> m_foundEvents;
    QList<Event> m_modifiedEvents;
    QScopedPointer<EventModelFilter> m_model;
    QScopedPointer<Ui::FindAndReplaceEventsDialog> m_ui;
};

#endif // FINDANDREPLACEEVENTSDIALOG_H
