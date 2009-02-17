#include <algorithm>

#include <QSettings>
#include <QCloseEvent>
#include <QtAlgorithms>
#include <QKeyEvent>

#include "Core/TimeSpans.h"

#include "Application.h"
#include "ViewHelpers.h"

#include "TimeTrackingView.h"
#include "Reports/CharmReport.h"

#include "ui_TimeTrackingView.h"

TimeTrackingView::TimeTrackingView( QWidget* parent )
    : CharmWindow( tr( "Time Tracker" ), parent )
    , m_ui( new Ui::TimeTrackingView )
{
    setWindowNumber( 3 );
    setWindowIdentifier( tr( "window_tracking" ) );
     QWidget* widget = new QWidget( this );
    m_ui->setupUi( widget );
    setCentralWidget( widget );
    connect( m_ui->summaryWidget, SIGNAL( maybeShrink() ),
             SLOT( slotMaybeShrink() ), Qt::QueuedConnection );
    connect( m_ui->summaryWidget, SIGNAL( startEvent( TaskId ) ),
             SLOT( slotStartEvent( TaskId ) ) );
    connect( m_ui->summaryWidget, SIGNAL( stopEvent() ),
             SLOT( slotStopEvent() ) );
}


TimeTrackingView::~TimeTrackingView()
{
    DATAMODEL->unregisterAdapter( this );
    delete m_ui; m_ui = 0;
}

void TimeTrackingView::slotShowHide()
{
    if ( isVisible() ) {
        hide();
    } else {
        restore();
        raise();
    }
}

TimeTrackingSummaryWidget* TimeTrackingView::summaryWidget()
{
    Q_ASSERT( m_ui );
    return m_ui->summaryWidget;
}

void TimeTrackingView::stateChanged( State previous )
{
    switch( Application::instance().state() ) {
    case Connecting: {
        connect( &Application::instance().timeSpans(), SIGNAL( timeSpansChanged() ),
                 SLOT( slotSelectTasksToShow() ) );
        DATAMODEL->registerAdapter( this );
        // restore Gui state:
        QSettings settings;
        if ( settings.contains( MetaKey_TimeTrackerGeometry ) ) {
            restoreGeometry( settings.value( MetaKey_TimeTrackerGeometry ).toByteArray() );
        }
        // restore visibility
        if ( settings.contains( MetaKey_TimeTrackerVisible ) ) {
            const bool visible = settings.value( MetaKey_TimeTrackerVisible ).toBool();
            if ( visible ) {
                show();
            } else {
                hide();
            }
        }
        summaryWidget()->handleActiveEvents();
        break;
    }
    case ShuttingDown:
        setEnabled( false );
        break;
    case Disconnecting: {
        // save Gui state:
        QSettings settings;
        settings.setValue( MetaKey_TimeTrackerGeometry, saveGeometry() );
        settings.setValue( MetaKey_TimeTrackerVisible, isVisible() );
        break;
    }
    default:
        break;
    }
}

void TimeTrackingView::showEvent( QShowEvent* )
{
    emit visibilityChanged( true );
}

void TimeTrackingView::hideEvent( QHideEvent* )
{
    emit visibilityChanged( false );
}

void TimeTrackingView::keyPressEvent( QKeyEvent* event )
{
    if ( event->type() == QEvent::KeyPress ) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>( event );
        if ( keyEvent->modifiers() & Qt::ControlModifier
             && keyEvent->key() == Qt::Key_W ) {
            // we must be visible, otherwise we would not get the event
            slotShowHide();
        }
    }
    QWidget::keyPressEvent( event );
}

void TimeTrackingView::saveConfiguration()
{
}

void TimeTrackingView::sendCommand( CharmCommand* )
{
    Q_ASSERT( false ); // should not be called
}

void TimeTrackingView::commitCommand( CharmCommand* )
{
}

void TimeTrackingView::restore()
{
    show();
}

void TimeTrackingView::quit()
{
}

// model adapter:
void TimeTrackingView::resetTasks()
{
    slotSelectTasksToShow();
}

void TimeTrackingView::taskAboutToBeAdded( TaskId parent, int pos )
{
}

void TimeTrackingView::taskAdded( TaskId id )
{
    slotSelectTasksToShow();
}

void TimeTrackingView::taskModified( TaskId id )
{
    slotSelectTasksToShow();
}

void TimeTrackingView::taskParentChanged( TaskId task, TaskId oldParent, TaskId newParent )
{
    slotSelectTasksToShow();
}

void TimeTrackingView::taskAboutToBeDeleted( TaskId )
{
}

void TimeTrackingView::taskDeleted( TaskId id )
{
    slotSelectTasksToShow();
}

void TimeTrackingView::resetEvents()
{
    slotSelectTasksToShow();
}

void TimeTrackingView::eventAboutToBeAdded( EventId id )
{
}

void TimeTrackingView::eventAdded( EventId id )
{
    slotSelectTasksToShow();
}

void TimeTrackingView::eventModified( EventId id, Event discardedEvent )
{
    slotSelectTasksToShow();
}

void TimeTrackingView::eventAboutToBeDeleted( EventId id )
{
}

void TimeTrackingView::eventDeleted( EventId id )
{
    slotSelectTasksToShow();
}

void TimeTrackingView::eventActivated( EventId id )
{
    summaryWidget()->handleActiveEvents();
}

void TimeTrackingView::eventDeactivated( EventId id )
{
    summaryWidget()->handleActiveEvents();
}

void TimeTrackingView::slotSelectTasksToShow()
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
        setFixedHeight( minHeight );
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

void TimeTrackingView::slotStopEvent()
{
    DATAMODEL->endAllEventsRequested();
}

#include "TimeTrackingView.moc"
