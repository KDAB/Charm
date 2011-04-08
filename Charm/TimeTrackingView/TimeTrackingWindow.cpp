#include <algorithm>

#include <QSettings>
#include <QCloseEvent>
#include <QtAlgorithms>
#include <QKeyEvent>
#include <QMenuBar>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>

#include "Core/TimeSpans.h"
#include "Core/TaskListMerger.h"
#include "Core/XmlSerialization.h"

#include "Application.h"
#include "ViewHelpers.h"
#include "TimeTrackingView.h"
#include "TimeTrackingWindow.h"
#include "Uniquifier.h"
#include "MakeTemporarilyVisible.h"
#include "CharmPreferences.h"
#include "CharmAboutDialog.h"
#include "Reports/ReportConfigurationPage.h"
#include "Commands/CommandExportToXml.h"
#include "Commands/CommandSetAllTasks.h"
#include "Commands/CommandModifyEvent.h"
#include "Commands/CommandImportFromXml.h"
#include "Idle/IdleDetector.h"
#include "Idle/IdleCorrectionDialog.h"


TimeTrackingWindow::TimeTrackingWindow( QWidget* parent )
    : CharmWindow( tr( "Time Tracker" ), parent )
    , m_summaryWidget( new TimeTrackingView( toolBar(), this ) )
    , m_reportDialog( this )
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

void TimeTrackingWindow::insertEditMenu()
{
    QMenu* editMenu = menuBar()->addMenu( tr( "Edit" ) );
    m_summaryWidget->populateEditMenu( editMenu );
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

void TimeTrackingWindow::slotEditPreferences( bool )
{
    MakeTemporarilyVisible m( this );
    CharmPreferences dialog( CONFIGURATION, this );

    if ( dialog.exec() ) {
        CONFIGURATION.eventsInLeafsOnly = dialog.eventsInLeafsOnly();
        CONFIGURATION.oneEventAtATime = dialog.oneEventAtATime();
        CONFIGURATION.taskTrackerFontSize = dialog.taskTrackerFontSize();
        CONFIGURATION.always24hEditing = dialog.always24hEditing();
        CONFIGURATION.durationFormat = dialog.durationFormat();
        CONFIGURATION.toolButtonStyle = dialog.toolButtonStyle();
        CONFIGURATION.detectIdling = dialog.detectIdling();
        CONFIGURATION.animatedTrayIcon = dialog.animatedTrayIcon();
        emit saveConfiguration();
    }
}

void TimeTrackingWindow::slotAboutDialog()
{
    MakeTemporarilyVisible m( this );
    CharmAboutDialog dialog( this );
    dialog.exec();
}

void TimeTrackingWindow::slotReportDialog()
{
    MakeTemporarilyVisible m( this );
    m_reportDialog.back();
    if ( m_reportDialog.exec() ) {
        ReportConfigurationPage* page = m_reportDialog.selectedPage();
        QDialog* preview = page->makeReportPreviewDialog( this );
        // preview is destroy-on-close and non-modal:
        preview->show();
    }
}

void TimeTrackingWindow::slotExportToXml()
{
    MakeTemporarilyVisible m( this );
    // ask for a filename:
    QSettings settings;
    QString path;
    if ( settings.contains( MetaKey_ExportToXmlRecentSavePath ) ) {
        path = settings.value( MetaKey_ExportToXmlRecentSavePath ).toString();
        QDir dir( path );
        if ( !dir.exists() ) path = QString();
    }

    QString filename = QFileDialog::getSaveFileName( this, tr( "Enter File Name" ), path );
    if ( filename.isEmpty() ) return;

    QFileInfo fileinfo( filename );
    path = fileinfo.absolutePath();

    if ( !path.isEmpty() ) {
        settings.setValue( MetaKey_ExportToXmlRecentSavePath, path );
    }

    if ( fileinfo.suffix().isEmpty() ) {
        filename+=".charmdatabaseexport";
    }

    // get a XML export:
    CommandExportToXml* command = new CommandExportToXml( filename, this );
    sendCommand( command );
}

void TimeTrackingWindow::slotImportFromXml()
{
    MakeTemporarilyVisible m( this );
    // ask for the filename:
    QSettings settings;
    QString path;
    if ( settings.contains( MetaKey_ExportToXmlRecentSavePath ) ) {
        path = settings.value( MetaKey_ExportToXmlRecentSavePath ).toString();
        QDir dir( path );
        if ( !dir.exists() ) path = QString();
    }

    QString filename = QFileDialog::getOpenFileName( this, tr( "Please Select File" ), path );
    if ( filename.isEmpty() ) return;
    QFileInfo fileinfo( filename );
    Q_ASSERT( fileinfo.exists() );

    // warn the user about the consequences:
    if ( QMessageBox::warning( this, tr("Watch out!" ),
                               tr("During import, all existing tasks and events will be deleted"
                                  " and replaced with the imported ones. Are you sure?" ),
                               QMessageBox::Yes | QMessageBox::No ) != QMessageBox::Yes )
        return;

    // ask the controller to import the file:
    CommandImportFromXml* cmd = new CommandImportFromXml( filename, this );
    sendCommand( cmd );
}

void TimeTrackingWindow::slotImportTasks()
{
    MakeTemporarilyVisible m( this );
    QString filename = QFileDialog::getOpenFileName( this, tr( "Please Select File" ), "", tr("Task definitions (*.xml);;All Files (*)") );
    if ( filename.isEmpty() ) return;
    QFileInfo fileinfo( filename );
    Q_ASSERT( fileinfo.exists() );


    TaskExport exporter;
    TaskListMerger merger;
    try {
        exporter.readFrom( filename );
        merger.setOldTasks( DATAMODEL->getAllTasks() );
        merger.setNewTasks( exporter.tasks() );
        if ( merger.modifiedTasks().count() == 0 && merger.addedTasks().count() == 0 ) {
            QMessageBox::information( this, tr( "Tasks Import" ), tr( "The selected task file does not contain any updates." ) );
        } else {
            QString detailsText(
                tr( "Importing this task list will result in %1 modified and %2 added tasks. Do you want to continue?" )
                .arg( merger.modifiedTasks().count() )
                .arg( merger.addedTasks().count() ) );
            if ( QMessageBox::warning( this, tr( "Tasks Import" ), detailsText,
                                       QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes )
                 == QMessageBox::Yes ) {
                CommandSetAllTasks* cmd = new CommandSetAllTasks( merger.mergedTaskList(), this );
                sendCommand( cmd );
            }
        }
    } catch(  CharmException& e ) {
        const QString message = e.what().isEmpty()
                                ?  tr( "The selected task definitions are invalid and cannot be imported." )
                                    : tr( "There was an error importing the task definitions:<br />%1" ).arg( e.what() );
        QMessageBox::critical( this, tr(  "Invalid Task Definitions" ), message);
        return;
    }
}

void TimeTrackingWindow::maybeIdle()
{
    static bool inProgress = false;
    if ( Application::instance().idleDetector() == 0 ) return;

    if ( inProgress == true ) return;
    Uniquifier u( &inProgress );

    IdleDetector* detector = Application::instance().idleDetector();

    Q_FOREACH( const IdleDetector::IdlePeriod& p, detector->idlePeriods() ) {
        qDebug() << "Application::slotMaybeIdle: computer might be have been idle from"
                 << p.first
                 << "to" << p.second;
    }

    // handle idle merging:
    IdleCorrectionDialog dialog( this );
    MakeTemporarilyVisible m( this );

    dialog.exec();
    switch( dialog.result() ) {
    case IdleCorrectionDialog::Idle_Ignore:
        break;
    case IdleCorrectionDialog::Idle_EndEvent: {
        EventIdList activeEvents = DATAMODEL->activeEvents();
        DATAMODEL->endAllEventsRequested();
        // FIXME with this option, we can only change the events to
        // the start time of one idle period, I chose to use the last
        // one:
        Q_ASSERT( !detector->idlePeriods().isEmpty() );
        const IdleDetector::IdlePeriod period = detector->idlePeriods().last();

        Q_FOREACH ( EventId eventId, activeEvents ) {
            Event event = DATAMODEL->eventForId( eventId );
            if ( event.isValid() ) {
                QDateTime start  = period.first; // initializes a valid QDateTime
                event.setEndDateTime( qMax( event.startDateTime(), start ) );
                Q_ASSERT( event.isValid() );
                CommandModifyEvent* cmd = new CommandModifyEvent( event, this );
                emit emitCommand( cmd );
            }
        }
        break;
    }
    default:
        break; // should not happen
    }
    detector->clear();
}

#include "TimeTrackingWindow.moc"
