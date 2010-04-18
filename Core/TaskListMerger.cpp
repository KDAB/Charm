#include "TaskListMerger.h"

TaskListMerger::TaskListMerger()
    : m_resultsValid( false )
{
}

void TaskListMerger::setOldTasks( const TaskList& tasks )
{
    verifyTaskList( tasks );
    m_oldTasks = tasks;
    for ( TaskList::iterator it = m_oldTasks.begin(); it != m_oldTasks.end(); ++it ) {
        ( *it ).setSubscribed( false );
    }
    qSort( m_oldTasks.begin(), m_oldTasks.end(), Task::lowerTaskId );
    m_resultsValid = false;
}

void TaskListMerger::setNewTasks( const TaskList& tasks )
{
    verifyTaskList( tasks );
    m_newTasks = tasks;
    for ( TaskList::iterator it = m_newTasks.begin(); it != m_newTasks.end(); ++it ) {
        ( *it ).setSubscribed( false );
    }
    qSort( m_newTasks.begin(), m_newTasks.end(), Task::lowerTaskId );
    m_resultsValid = false;
}

void TaskListMerger::calculateResults() const
{
    if ( m_resultsValid ) return;

    // insert sentinels at end of list:
    const TaskId maxId = qMax(
        m_oldTasks.isEmpty() ? 0 : m_oldTasks.last().id(),
        m_newTasks.isEmpty() ? 0 : m_newTasks.last().id() );
    const TaskId sentinelId = maxId + 1;
    const Task sentinel( sentinelId, QObject::tr( "Sentinel Task" ) );

    TaskList oldTasks( m_oldTasks );
    TaskList newTasks( m_newTasks );
    oldTasks << sentinel;
    newTasks << sentinel;

    TaskList::iterator oldIt = oldTasks.begin();
    TaskList::iterator newIt = newTasks.begin();

    do {
        if ( ( *oldIt ).id() < ( *newIt ).id() ) {
            // there is a task in the old task list that is not an
            // element of the new task list, ignore (the user added it
            // manually)
            ++oldIt;
        } else if ( ( *oldIt ).id() == ( *newIt ).id() ) {
            //
            if ( *oldIt != *newIt ) {
                m_modifiedTasks << ( *oldIt );
                *oldIt = *newIt;
            }
            ++oldIt;
            ++newIt;
        } else {
            // there are tasks in newtasks that are not in oldtasks,
            // so they are new
            m_addedTasks << *newIt;
            ++newIt;
        }
    } while ( oldIt != oldTasks.end() || newIt != newTasks.end() );

    oldTasks.pop_back(); // remove sentinel
    m_results = oldTasks + m_addedTasks;

    // one last check: if tasks where modified through the new task
    // lists, maybe local-only tasks have become orphans?
    if ( ! Task::checkForUniqueTaskIds( m_results ) ) {
        throw InvalidTaskListException( QObject::tr( "the merged task list is invalid, it contains duplicate task ids" ) );
    }

    if ( ! Task::checkForTreeness( m_results ) ) {
        throw InvalidTaskListException( QObject::tr( "the merged tasks database is not a directed graph, this is seriously bad, go fix it" ) );
    }

    m_resultsValid = true;
}

void TaskListMerger::verifyTaskList( const TaskList& tasks )
{
    if ( ! Task::checkForUniqueTaskIds( tasks ) ) {
        throw InvalidTaskListException( QObject::tr( "task list contains duplicate task ids" ) );
    }

    if ( ! Task::checkForTreeness( tasks ) ) {
        throw InvalidTaskListException( QObject::tr( "task list is not a directed graph, this is seriously bad, go fix it" ) );
    }
}

TaskList TaskListMerger::addedTasks() const
{
    calculateResults();
    return m_addedTasks;
}

TaskList TaskListMerger::modifiedTasks() const
{
    calculateResults();
    return m_modifiedTasks;
}


TaskList TaskListMerger::mergedTaskList() const
{
    calculateResults();
    return m_results;
}




