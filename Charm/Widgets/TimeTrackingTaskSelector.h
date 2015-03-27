/*
  TimeTrackingTaskSelector.h

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

#ifndef TIMETRACKINGTASKSELECTOR_H
#define TIMETRACKINGTASKSELECTOR_H

#include <QWidget>
#include <QVector>
#include <QDialog>

#include "Core/Event.h"
#include "Core/Task.h"

#include "WeeklySummary.h"

class QAction;
class QMenu;
class QToolButton;
class QTextEdit;
class QToolBar;

class TimeTrackingTaskSelector : public QWidget
{
    Q_OBJECT
public:
    explicit TimeTrackingTaskSelector(QWidget *parent = nullptr);

    void populate( const QVector<WeeklySummary>& summaries  );
    void handleActiveEvents();
    void taskSelected( const WeeklySummary& );

    void resizeEvent( QResizeEvent* ) override;
    QSize sizeHint() const override;
    QMenu* menu() const;

    void populateEditMenu( QMenu* );

signals:
    void startEvent( TaskId );
    void stopEvents();
    void updateSummariesPlease();

private slots:
    void slotActionSelected( QAction* );
    void slotGoStopToggled( bool );
    void slotEditCommentClicked();
    void slotManuallySelectTask();

private:
    void taskSelected( const QString& taskname, TaskId id );
    QToolButton* m_stopGoButton;
    QAction* m_stopGoAction;
    QToolButton* m_editCommentButton;
    QAction* m_editCommentAction;
    QToolButton* m_taskSelectorButton;
    QAction* m_startOtherTaskAction;
    QMenu *m_menu;
    /** The task that has been selected from the menu. */
    TaskId m_selectedTask;
    /** If the user selected a task through the "Select other task..." menu action,
      its Id is stored here. */
    TaskId m_manuallySelectedTask;
    /** Temporarily store that a task has been manually selected, so that it can be
      activated in the menu once after selection. */
    bool m_taskManuallySelected;
};

#endif // TIMETRACKINGTASKSELECTOR_H
