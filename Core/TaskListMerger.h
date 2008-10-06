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
