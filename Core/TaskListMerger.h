/*
  TaskListMerger.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2008-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>

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

#ifndef TASKLISTMERGER_H
#define TASKLISTMERGER_H

#include "Task.h"

/** TaskListMerger merges two task lists into a single one.
 *
 * It merges the existing tasks in oldtasks with the possibly new
 * state in newtasks.
 *
 * There are a number of assumptions in the merge algorithm: First of
 * all, it assumes tasks never get deleted. Instead of deleting them,
 * they would be marked as expired by setting their valid-until
 * date. Second (based on the first assumption), tasks that exist in
 * oldtasks but not in newtasks are assumed to be locally added, and
 * left untouched. Third, tasks are recognized by their task id.
 *
 * If a task id exists in newtasks, but not in oldtasks, the task is
 * assumed new. If a task id exists in both lists, but differs,
 * newtasks is assumed to contain the newer state, and the returned
 * list will contain the task as it was in newtasks.
 */
class TaskListMerger
{
public:
    TaskListMerger();

    void setOldTasks( const TaskList& tasks );
    void setNewTasks( const TaskList& tasks );

    TaskList mergedTaskList() const;
    TaskList addedTasks() const;
    TaskList modifiedTasks() const;

private:
    void verifyTaskList( const TaskList& tasks );
    void calculateResults() const;

    mutable bool m_resultsValid;
    TaskList m_oldTasks;
    TaskList m_newTasks;
    mutable TaskList m_results;
    mutable TaskList m_addedTasks;
    mutable TaskList m_modifiedTasks;
};

#endif
