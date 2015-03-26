/*
  TaskTreeItem.h

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

#ifndef TASKTREEITEM_H
#define TASKTREEITEM_H

#include <QList>

#include "Task.h"

/** TaskTreeItem is a node in the task tree.
    The tasks form a tree, since tasks can belong to a parent task.
    This structure is modeled by storing TaskTreeItems in a map that
    associates them with the respective task id.
    Every TaskTreeItem has a parent that represents the parent
    task. If a task has no parent, it is a child of the root
    TaskTreeItem stored in the model.
    Every TaskTreeItem keeps a list of children.
    Every TaskTreeItem also has a position in it's parents list of
    children. This integer position can be retrieved by calling row on
    the item.
*/
class TaskTreeItem
{
public:
    typedef QList<const TaskTreeItem*> ConstPointerList;
    typedef std::map<TaskId, TaskTreeItem> Map;

    TaskTreeItem();

    explicit TaskTreeItem( const Task& task, TaskTreeItem* parent = nullptr );
    TaskTreeItem( const TaskTreeItem& other );
    TaskTreeItem& operator=( const TaskTreeItem& other );

    ~TaskTreeItem();

    void makeChildOf( TaskTreeItem& parent );

    bool isValid() const;

    Task& task();

    const Task& task() const;

    const TaskTreeItem& child( int row ) const;

    int row() const;

    int childCount() const;

    // recursively find all children of this item
    // warning: SLOW
    TaskList children() const;

    TaskIdList childIds() const;

private:
    TaskTreeItem* m_parent;
    ConstPointerList m_children;
    Task m_task;
};


#endif
