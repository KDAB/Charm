#include "WeeklySummary.h"

#include "Core/CharmDataModel.h"
#include "Core/Event.h"
#include "Core/Task.h"

static const int DAYS_IN_WEEK = 7;

WeeklySummary::WeeklySummary()
    : task( 0 ), durations( DAYS_IN_WEEK, 0 ) {
}

QVector<WeeklySummary> WeeklySummary::summariesForTimespan( CharmDataModel* dataModel, const TimeSpan& timespan )
{
    const EventIdList eventIds = dataModel->eventsThatStartInTimeFrame( timespan );
    // prepare a list of unique task ids used within the time span:
    TaskIdList taskIds, uniqueTaskIds; // the list of tasks to show
    EventList events;
    Q_FOREACH( EventId id, eventIds ) {
        Event event = dataModel->eventForId( id );
        events << event;
        taskIds << event.taskId();
    }
    qSort( taskIds );
    std::unique_copy( taskIds.begin(), taskIds.end(), std::back_inserter( uniqueTaskIds ) );
    Q_ASSERT( events.size() == eventIds.size() );
    // retrieve task information
    QVector<WeeklySummary> summaries( uniqueTaskIds.size() );
    for ( int i = 0; i < uniqueTaskIds.size(); ++i ) {
        summaries[i].task = uniqueTaskIds.at( i );
        const Task& task = dataModel->getTask( uniqueTaskIds[i] );
        summaries[i].taskname = dataModel->fullTaskName( task );
    }
    // now add the times to the tasks:
    Q_FOREACH( const Event& event, events ) {
        // find the index for this event:
        TaskIdList::iterator it = std::find( uniqueTaskIds.begin(), uniqueTaskIds.end(), event.taskId() );
        if ( it != uniqueTaskIds.end() ) {
            const int index = std::distance( uniqueTaskIds.begin(), it );
            Q_ASSERT( index >= 0 && index < summaries.size() );
            const int dayOfWeek = event.startDateTime().date().dayOfWeek() - 1;
            Q_ASSERT( dayOfWeek >= 0 && dayOfWeek < 7 );
            summaries[index].durations[dayOfWeek] += event.duration();
        }
    }

    return summaries;
}
