#include <QtAlgorithms>

#include "ViewHelpers.h"

#include "TimeTrackingView.h"
#include "Reports/CharmReport.h"

#include "ui_TimeTrackingView.h"

TimeTrackingView::TimeTrackingView( QWidget* parent )
    : QWidget( parent )
    , m_ui( new Ui::TimeTrackingView )
{
    m_ui->setupUi( this );
    connect( m_ui->summaryWidget, SIGNAL( maybeShrink() ),
             SLOT( slotMaybeShrink() ), Qt::QueuedConnection );
    connect( m_ui->summaryWidget, SIGNAL( startEvent( TaskId ) ),
             SLOT( slotStartEvent( TaskId ) ) );
    connect( m_ui->summaryWidget, SIGNAL( stopEvent( TaskId ) ),
             SLOT( slotStopEvent( TaskId ) ) );
}


TimeTrackingView::~TimeTrackingView()
{
    DATAMODEL->unregisterAdapter( this );
    delete m_ui; m_ui = 0;
}

TimeTrackingSummaryWidget* TimeTrackingView::summaryWidget()
{
    Q_ASSERT( m_ui );
    return m_ui->summaryWidget;
}

void TimeTrackingView::stateChanged( State previous )
{
    if ( previous == Constructed ) {
        DATAMODEL->registerAdapter( this );
    }
}

void TimeTrackingView::saveConfiguration()
{
}

void TimeTrackingView::emitCommand( CharmCommand* )
{
}

void TimeTrackingView::sendCommand( CharmCommand* )
{
}

void TimeTrackingView::commitCommand( CharmCommand* )
{
}

void TimeTrackingView::restore()
{
}

void TimeTrackingView::quit()
{
}

// model adapter:
void TimeTrackingView::resetTasks()
{
}

void TimeTrackingView::taskAboutToBeAdded( TaskId parent, int pos )
{
}

void TimeTrackingView::taskAdded( TaskId id )
{
}

void TimeTrackingView::taskModified( TaskId id )
{
}

void TimeTrackingView::taskParentChanged( TaskId task, TaskId oldParent, TaskId newParent )
{
}

void TimeTrackingView::taskAboutToBeDeleted( TaskId )
{
}

void TimeTrackingView::taskDeleted( TaskId id )
{
}

void TimeTrackingView::resetEvents()
{
    selectTasksToShow();
}

void TimeTrackingView::eventAboutToBeAdded( EventId id )
{
}

void TimeTrackingView::eventAdded( EventId id )
{
    selectTasksToShow();
}

void TimeTrackingView::eventModified( EventId id, Event discardedEvent )
{
    selectTasksToShow();
}

void TimeTrackingView::eventAboutToBeDeleted( EventId id )
{
}

void TimeTrackingView::eventDeleted( EventId id )
{
    selectTasksToShow();
}

void TimeTrackingView::eventActivated( EventId id )
{
}

void TimeTrackingView::eventDeactivated( EventId id )
{
}

void TimeTrackingView::selectTasksToShow()
{
    // we would like to always show some tasks, if there are any
    // first, we select tasks that most recently where active
    //
    // find this weeks time span, and retrieve the events matching:
    const NamedTimeSpan thisWeek = Application::instance().timeSpans().thisWeek();
    const EventIdList eventIds = DATAMODEL->eventsThatStartInTimeFrame( thisWeek.timespan );
    // prepare a list of unique task ids used within the time span:
    TaskIdList taskIds, uniqueTaskIds; // the list of tasks to show
    EventList events;
    Q_FOREACH( EventId id, eventIds ) {
        Event event = DATAMODEL->eventForId( id );
        events << event;
        taskIds << event.taskId();
    }
    qSort( taskIds );
    std::unique_copy( taskIds.begin(), taskIds.end(), std::back_inserter( uniqueTaskIds ) );
    Q_ASSERT( events.size() == eventIds.size() );
    // retrieve task information
    QVector<TimeTrackingSummaryWidget::WeeklySummary> summaries( uniqueTaskIds.size() );
    for ( int i = 0; i < uniqueTaskIds.size(); ++i ) {
        summaries[i].task = uniqueTaskIds.at( i );
        const Task& task = DATAMODEL->getTask( uniqueTaskIds[i] );
        summaries[i].taskname = tasknameWithParents( task );
    }
    // now add the times to the tasks:
    Q_FOREACH( const Event& event, events ) {
        // find the index for this event:
        TaskIdList::iterator it = std::find( uniqueTaskIds.begin(), uniqueTaskIds.end(), event.taskId() );
        if ( it != uniqueTaskIds.end() ) {
            int index = std::distance( uniqueTaskIds.begin(), it );
            Q_ASSERT( index >= 0 && index < summaries.size() );
            const int dayOfWeek = event.startDateTime().date().dayOfWeek() - 1;
            Q_ASSERT( dayOfWeek >= 0 && dayOfWeek < 7 );
            summaries[index].durations[dayOfWeek] += event.duration();
        }
    }
    // and update the widget:
    m_summaries = summaries;
    summaryWidget()->setSummaries( m_summaries );
}

void TimeTrackingView::slotMaybeShrink()
{
    const int minHeight = minimumSize().height();
    if ( minHeight < height() ) {
        resize( width(), minHeight );
    }
}

void TimeTrackingView::slotStartEvent( TaskId id )
{
    const TaskTreeItem& item = DATAMODEL->taskTreeItem( id );
    if ( CONFIGURATION.eventsInLeafsOnly && item.childCount() > 0 ) {
        qDebug() << "FIXME this should be prevented by disabling the menu items";
        return;
    }

    if( item.task().isCurrentlyValid() ) {
        DATAMODEL->startEventRequested( item.task() );
    }
}

void TimeTrackingView::slotStopEvent( TaskId id )
{
    const TaskTreeItem& item = DATAMODEL->taskTreeItem( id );
    DATAMODEL->endEventRequested( item.task() );
}

#include "TimeTrackingView.moc"
