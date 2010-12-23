#include <algorithm>

#include <QSettings>
#include <QCloseEvent>
#include <QtAlgorithms>
#include <QKeyEvent>

#include "Core/TimeSpans.h"

#include "Application.h"
#include "ViewHelpers.h"
#include "TimeTrackingView.h"

#include "TimeTrackingWindow.h"

TimeTrackingWindow::TimeTrackingWindow( QWidget* parent )
    : CharmWindow( tr( "Time Tracker" ), parent )
    , m_summaryWidget( new TimeTrackingView( toolBar(), this ) )
{
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    setWindowNumber( 3 );
    setWindowIdentifier( QLatin1String( "window_tracking" ) );
    setCentralWidget( m_summaryWidget );
    connect( m_summaryWidget, SIGNAL( startEvent( TaskId ) ),
             SLOT( slotStartEvent( TaskId ) ) );
    connect( m_summaryWidget, SIGNAL( stopEvents() ),
             SLOT( slotStopEvent() ) );
}

void TimeTrackingWindow::showEvent( QShowEvent* e )
{
    CharmWindow::showEvent( e );
}

QMenu* TimeTrackingWindow::menu() const
{
    return m_summaryWidget->menu();
}

TimeTrackingWindow::~TimeTrackingWindow()
{
    DATAMODEL->unregisterAdapter( this );
}

void TimeTrackingWindow::stateChanged( State previous )
{
    CharmWindow::stateChanged( previous );
    switch( Application::instance().state() ) {
    case Connecting: {
        connect( &Application::instance().timeSpans(), SIGNAL( timeSpansChanged() ),
                 SLOT( slotSelectTasksToShow() ) );
        DATAMODEL->registerAdapter( this );
        m_summaryWidget->setSummaries( QVector<WeeklySummary>() );
        m_summaryWidget->handleActiveEvents();
        break;
    }
    case Disconnecting:
    case ShuttingDown:
    default:
        break;
    }
}

void TimeTrackingWindow::sendCommand( CharmCommand* )
{
    Q_ASSERT( false ); // should not be called
}

void TimeTrackingWindow::commitCommand( CharmCommand* )
{
}

void TimeTrackingWindow::restore()
{
    show();
}

void TimeTrackingWindow::quit()
{
}

// model adapter:
void TimeTrackingWindow::resetTasks()
{
    slotSelectTasksToShow();
}

void TimeTrackingWindow::taskAboutToBeAdded( TaskId parent, int pos )
{
}

void TimeTrackingWindow::taskAdded( TaskId id )
{
    slotSelectTasksToShow();
}

void TimeTrackingWindow::taskModified( TaskId id )
{
    slotSelectTasksToShow();
}

void TimeTrackingWindow::taskParentChanged( TaskId task, TaskId oldParent, TaskId newParent )
{
    slotSelectTasksToShow();
}

void TimeTrackingWindow::taskAboutToBeDeleted( TaskId )
{
}

void TimeTrackingWindow::taskDeleted( TaskId id )
{
    slotSelectTasksToShow();
}

void TimeTrackingWindow::resetEvents()
{
    slotSelectTasksToShow();
}

void TimeTrackingWindow::eventAboutToBeAdded( EventId id )
{
}

void TimeTrackingWindow::eventAdded( EventId id )
{
    slotSelectTasksToShow();
}

void TimeTrackingWindow::eventModified( EventId id, Event discardedEvent )
{
    slotSelectTasksToShow();
}

void TimeTrackingWindow::eventAboutToBeDeleted( EventId id )
{
}

void TimeTrackingWindow::eventDeleted( EventId id )
{
    slotSelectTasksToShow();
}

void TimeTrackingWindow::eventActivated( EventId id )
{
    m_summaryWidget->handleActiveEvents();
}

void TimeTrackingWindow::eventDeactivated( EventId id )
{
    m_summaryWidget->handleActiveEvents();
}

void TimeTrackingWindow::slotSelectTasksToShow()
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
    QVector<WeeklySummary> summaries( uniqueTaskIds.size() );
    for ( int i = 0; i < uniqueTaskIds.size(); ++i ) {
        summaries[i].task = uniqueTaskIds.at( i );
        const Task& task = DATAMODEL->getTask( uniqueTaskIds[i] );
        summaries[i].taskname = DATAMODEL->fullTaskName( task );
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
    // and update the widget:
    m_summaries = summaries;
    m_summaryWidget->setSummaries( m_summaries );
}

void TimeTrackingWindow::slotStartEvent( TaskId id )
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

void TimeTrackingWindow::slotStopEvent()
{
    DATAMODEL->endAllEventsRequested();
}

#include "TimeTrackingWindow.moc"
