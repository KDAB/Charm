/*
  TasksView.h

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

#ifndef TASKSVIEW_H
#define TASKSVIEW_H

#include <QWidget>
#include <QAction>

#include <Core/Event.h>
#include <Core/State.h>
#include <Core/CommandEmitterInterface.h>

#include "ViewModeInterface.h"

class QMenu;
class TasksViewDelegate;
class QToolBar;
class QTreeView;

class TasksView : public QWidget,
             public ViewModeInterface,
             public CommandEmitterInterface
{
    Q_OBJECT

public:
    explicit TasksView ( QToolBar* toolBar, QWidget* parent = nullptr );
    ~TasksView();

    // implement ViewModeInterface:
    void stateChanged( State previous ) override;
    void configurationChanged() override;
    void setModel( ModelConnector* ) override;

    void populateEditMenu( QMenu* );

public Q_SLOTS:
    void commitCommand( CharmCommand* ) override;

signals:
    // FIXME connect to MainWindow
    void saveConfiguration();
    void emitCommand( CharmCommand* );
    void emitCommandRollback( CharmCommand* );

private slots:
    void actionNewTask();
    void actionNewSubTask();
    void actionEditTask();
    void actionDeleteTask();
    void slotFiltertextChanged( const QString& filtertext );
    void taskPrefilteringChanged();
    void slotContextMenuRequested( const QPoint& );

    void slotEventActivated( EventId );
    void slotEventDeactivated( EventId );
    // this method is called every time the UI actions need update, for
    // example when the current index changes:
    void configureUi();
    void restoreGuiState() override;

private:
    void saveGuiState() override;

    // helper to retrieve selected task:
    Task selectedTask();
    void addTaskHelper( const Task& parent );

    TasksViewDelegate* m_delegate;
    QAction m_actionNewTask;
    QAction m_actionNewSubTask;
    QAction m_actionEditTask;
    QAction m_actionDeleteTask;
    QAction m_actionExpandTree;
    QAction m_actionCollapseTree;
    QAction* m_showCurrentOnly;
    QAction* m_showSubscribedOnly;
    QTreeView* m_treeView;
};

#endif
