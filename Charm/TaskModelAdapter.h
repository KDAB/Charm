/*
  TaskModelAdapter.h

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

#ifndef TASKMODELADAPTER_H
#define TASKMODELADAPTER_H

#include <QAbstractItemModel>
#include <QPointer>

#include "Core/TaskModelInterface.h"
#include "Core/CharmDataModel.h"
#include "Core/CharmDataModelAdapterInterface.h"
#include "Core/CommandEmitterInterface.h"

enum ViewColumns {
    Column_TaskId, // FIXME rename
    Column_TaskColumnCount
};

enum TasksViewRoles {
    TasksViewRole_Name = 0x1045F132,
    TasksViewRole_RunningTime,
    TasksViewRole_Comment,
    TasksViewRole_TaskId,
    TasksViewRole_Filter ///< Role for search/filter
};

typedef ViewColumns ViewColumn;

/** TaskModelAdapter adapts the CharmDataModel to be used in the task view
    (in main view and "select task" dialog).

    It is a QAbstractItemModel, and stores the TaskTreeItem pointer of
    the respective address in the model indexes internal pointer.
*/
class TaskModelAdapter :  public QAbstractItemModel,
                          public TaskModelInterface,
                          public CommandEmitterInterface,
                          public CharmDataModelAdapterInterface
{
    Q_OBJECT

public:
    explicit TaskModelAdapter( CharmDataModel* parent );
    ~TaskModelAdapter();

    // reimplement QAbstractItemModel:
    int columnCount( const QModelIndex& parent = QModelIndex() ) const override;
    int rowCount( const QModelIndex& parent = QModelIndex() ) const override;
    QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const override;
    QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const override;
    QModelIndex parent( const QModelIndex & index ) const override;
    // QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    Qt::ItemFlags flags( const QModelIndex & index ) const override;
    bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole ) override;

    // reimplement CharmDataModelAdapterInterface:
    void resetTasks() override;
    void taskAboutToBeAdded( TaskId parent, int pos ) override;
    void taskAdded( TaskId id ) override;
    void taskModified( TaskId id ) override;
    void taskParentChanged( TaskId task, TaskId oldParent, TaskId newParent ) override;
    void taskAboutToBeDeleted( TaskId ) override;
    void taskDeleted( TaskId id ) override;

    void resetEvents() override {}
    void eventAboutToBeAdded( EventId ) override {}
    void eventAdded( EventId ) override;
    void eventModified( EventId, Event ) override;
    void eventAboutToBeDeleted( EventId ) override {}
    void eventDeleted( EventId ) override;

    void eventActivated( EventId id ) override;
    void eventDeactivated( EventId id ) override;

    // reimplement TaskModelInterface:
    Task taskForIndex( const QModelIndex& ) const override;
    QModelIndex indexForTaskId( TaskId ) const override;
    bool taskIsActive( const Task& task ) const override;
    bool taskHasChildren( const Task& task ) const override;
    bool taskIdExists( TaskId taskId ) const override;
    TaskIdList childrenIds( const Task& task ) const;

    // reimplement CommandEmitterInterface:
    void commitCommand( CharmCommand* ) override;

signals:
    void eventActivationNotice( EventId id );
    void eventDeactivationNotice( EventId id );

private:
    const TaskTreeItem* itemFor ( const QModelIndex& ) const;
    QModelIndex indexForTaskTreeItem( const TaskTreeItem& item, int column = 0 ) const;

    QPointer<CharmDataModel> m_dataModel;
};

#endif
