#include <algorithm>

#include <QBuffer>
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
#include "MessageBox.h"
#include "EnterVacationDialog.h"
#include "ViewHelpers.h"
#include "TimeTrackingView.h"
#include "TimeTrackingWindow.h"
#include "Uniquifier.h"
#include "MakeTemporarilyVisible.h"
#include "CharmPreferences.h"
#include "CharmAboutDialog.h"
#include "Commands/CommandExportToXml.h"
#include "Commands/CommandSetAllTasks.h"
#include "Commands/CommandMakeEvent.h"
#include "Commands/CommandModifyEvent.h"
#include "Commands/CommandImportFromXml.h"
#include "Idle/IdleDetector.h"
#include "Idle/IdleCorrectionDialog.h"
#include "HttpClient/GetProjectCodesJob.h"
#include "Reports/ActivityReport.h"
#include "Reports/WeeklyTimesheet.h"
#include "Reports/MonthlyTimesheet.h"
#include "Reports/MonthlyTimesheetConfigurationDialog.h"

TimeTrackingWindow::TimeTrackingWindow( QWidget* parent )
    : CharmWindow( tr( "Time Tracker" ), parent )
    , m_weeklyTimesheetDialog( 0 )
    , m_monthlyTimesheetDialog( 0 )
    , m_activityReportDialog( 0 )
    , m_summaryWidget( new TimeTrackingView( toolBar(), this ) )
    , m_billDialog( new BillDialog( this ) )
{
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    setWindowNumber( 3 );
    setWindowIdentifier( QLatin1String( "window_tracking" ) );
    setCentralWidget( m_summaryWidget );
    connect( m_summaryWidget, SIGNAL( startEvent( TaskId ) ),
             SLOT( slotStartEvent( TaskId ) ) );
    connect( m_summaryWidget, SIGNAL( stopEvents() ),
             SLOT( slotStopEvent() ) );
    connect( &m_checkUploadedSheetsTimer, SIGNAL( timeout() ),
             SLOT( slotCheckUploadedTimesheets() ) );
    connect( m_billDialog, SIGNAL( finished(int) ),
             SLOT( slotBillGone(int) ) );
    //Check every 60 minutes if there are timesheets due
    m_checkUploadedSheetsTimer.setInterval(60 * 60 * 1000);
    if (CONFIGURATION.warnUnuploadedTimesheets)
        m_checkUploadedSheetsTimer.start();
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
    if ( Application::hasInstance() )
        DATAMODEL->unregisterAdapter( this );
}

void TimeTrackingWindow::stateChanged( State previous )
{
    CharmWindow::stateChanged( previous );
    switch( Application::instance().state() ) {
    case Connecting: {
        connect( Application::instance().dateChangeWatcher(), SIGNAL( dateChanged() ),
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

void TimeTrackingWindow::configurationChanged()
{
    if (CONFIGURATION.warnUnuploadedTimesheets)
        m_checkUploadedSheetsTimer.start();
    else
        m_checkUploadedSheetsTimer.stop();
    m_summaryWidget->configurationChanged();
    CharmWindow::configurationChanged();
}

void TimeTrackingWindow::slotSelectTasksToShow()
{
    // we would like to always show some tasks, if there are any
    // first, we select tasks that most recently where active
    const NamedTimeSpan thisWeek = TimeSpans().thisWeek();
    // and update the widget:
    m_summaries = WeeklySummary::summariesForTimespan( DATAMODEL, thisWeek.timespan );
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
        CONFIGURATION.timeTrackerFontSize = dialog.timeTrackerFontSize();
        CONFIGURATION.durationFormat = dialog.durationFormat();
        CONFIGURATION.toolButtonStyle = dialog.toolButtonStyle();
        CONFIGURATION.detectIdling = dialog.detectIdling();
        CONFIGURATION.warnUnuploadedTimesheets = dialog.warnUnuploadedTimesheets();
        emit saveConfiguration();
    }
}

void TimeTrackingWindow::slotAboutDialog()
{
    MakeTemporarilyVisible m( this );
    CharmAboutDialog dialog( this );
    dialog.exec();
}

void TimeTrackingWindow::slotEnterVacation()
{
    MakeTemporarilyVisible m( this );
    EnterVacationDialog dialog( this );
    if ( dialog.exec() != QDialog::Accepted )
        return;
    const EventList events = dialog.events();
    Q_FOREACH ( const Event& event, events ) {
        CommandMakeEvent* command = new CommandMakeEvent( event, this );
        sendCommand( command );
    }
}

void TimeTrackingWindow::slotActivityReport()
{
    delete m_activityReportDialog;
    m_activityReportDialog = new ActivityReportConfigurationDialog( this );
    m_activityReportDialog->setAttribute( Qt::WA_DeleteOnClose );
    connect( m_activityReportDialog, SIGNAL( finished( int ) ),
             this, SLOT( slotActivityReportPreview( int ) ) );
    m_activityReportDialog->show();
}

void TimeTrackingWindow::resetWeeklyTimesheetDialog()
{
    delete m_weeklyTimesheetDialog;
    m_weeklyTimesheetDialog = new WeeklyTimesheetConfigurationDialog( this );
    m_weeklyTimesheetDialog->setAttribute( Qt::WA_DeleteOnClose );
    connect( m_weeklyTimesheetDialog, SIGNAL( finished( int ) ),
             this, SLOT( slotWeeklyTimesheetPreview( int ) ) );
}

void TimeTrackingWindow::slotWeeklyTimesheetReport()
{
    resetWeeklyTimesheetDialog();
    m_weeklyTimesheetDialog->show();
}

void TimeTrackingWindow::resetMonthlyTimesheetDialog()
{
    delete m_monthlyTimesheetDialog;
    m_monthlyTimesheetDialog = new MonthlyTimesheetConfigurationDialog( this );
    m_monthlyTimesheetDialog->setAttribute( Qt::WA_DeleteOnClose );
    connect( m_monthlyTimesheetDialog, SIGNAL( finished( int ) ),
             this, SLOT( slotMonthlyTimesheetPreview( int ) ) );
}

void TimeTrackingWindow::slotMonthlyTimesheetReport()
{
    resetMonthlyTimesheetDialog();
    m_monthlyTimesheetDialog->show();
}

void TimeTrackingWindow::slotWeeklyTimesheetPreview( int result )
{
    showPreview( m_weeklyTimesheetDialog, result );
    m_weeklyTimesheetDialog = 0;
}

void TimeTrackingWindow::slotMonthlyTimesheetPreview( int result )
{
    showPreview( m_monthlyTimesheetDialog, result );
    m_monthlyTimesheetDialog = 0;
}

void TimeTrackingWindow::slotActivityReportPreview( int result )
{
    showPreview( m_activityReportDialog, result );
    m_activityReportDialog = 0;
}

void TimeTrackingWindow::showPreview( ReportConfigurationDialog* dialog, int result )
{
    if ( result == QDialog::Accepted )
        dialog->showReportPreviewDialog( this );
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
    if ( MessageBox::warning( this, tr( "Watch out!" ),
                              tr( "During import, all existing tasks and events will be deleted"
                                  " and replaced with the imported ones. Are you sure?" ), tr( "Delete" ), tr( "Cancel" ) ) != QMessageBox::Yes )
        return;

    // ask the controller to import the file:
    CommandImportFromXml* cmd = new CommandImportFromXml( filename, this );
    sendCommand( cmd );
}

void TimeTrackingWindow::slotSyncTasks()
{
    GetProjectCodesJob* client = new GetProjectCodesJob( this );
    client->setParentWidget( this );
    connect(client, SIGNAL(finished(HttpJob*)), this, SLOT(slotTasksDownloaded(HttpJob*)) );
    client->start();
}

void TimeTrackingWindow::slotTasksDownloaded( HttpJob* job_ )
{
    GetProjectCodesJob* job = qobject_cast<GetProjectCodesJob*>( job_ );
    Q_ASSERT( job );
    if ( job->error() == HttpJob::Canceled )
        return;

    if ( job->error() ) {
        QMessageBox::critical( this, tr("Error"), tr("Could not download the task list: %1").arg( job->errorString() ) );
        return;
    }

    QBuffer buffer;
    buffer.setData( job->payload() );
    buffer.open( QIODevice::ReadOnly );
    importTasksFromDeviceOrFile( &buffer, QString() );
}

void TimeTrackingWindow::slotImportTasks()
{
    const QString filename = QFileDialog::getOpenFileName( this, tr( "Please Select File" ), "",
                                                           tr("Task definitions (*.xml);;All Files (*)") );
    if ( filename.isNull() )
        return;
    importTasksFromDeviceOrFile( 0, filename );
}

void TimeTrackingWindow::slotExportTasks()
{
    const MakeTemporarilyVisible m( this );
    const QString filename = QFileDialog::getSaveFileName( this, tr( "Please select export filename" ), "",
                                                     tr("Task definitions (*.xml);;All Files (*)") );
    if ( filename.isNull() ) return;

    try {
        const TaskList tasks = DATAMODEL->getAllTasks();
        TaskExport::writeTo( filename, tasks );
    } catch ( const XmlSerializationException& e) {
        const QString message = e.what().isEmpty()
                ? tr( "Error exporting the task definitions!" )
                : tr( "There was an error exporting the task definitions:<br />%1" ).arg( e.what() );
        QMessageBox::critical( this, tr(  "Error during export" ), message);
        return;
    }
}

void TimeTrackingWindow::slotCheckUploadedTimesheets()
{
    WeeksByYear missing = missingTimeSheets();
    if (missing.isEmpty())
        return;
    m_checkUploadedSheetsTimer.stop();
    //The usual case is just one missing week, unless we've been giving Bill a hard time
    //Perhaps in the future Bill can bug us about more than one report at a time
    Q_ASSERT(!missing.begin().value().isEmpty());
    int year = missing.begin().key();
    int week = missing.begin().value().first();
    m_billDialog->setReport(year, week);
    m_billDialog->show();
    m_billDialog->raise();
    m_billDialog->activateWindow();
}

void TimeTrackingWindow::slotBillGone(int result)
{
    switch(result)
    {
    case BillDialog::AlreadyDone:
        addUploadedTimesheet( m_billDialog->year(), m_billDialog->week() );
        break;
    case BillDialog::AsYouWish:
        resetWeeklyTimesheetDialog();
        m_weeklyTimesheetDialog->setDefaultWeek(m_billDialog->year(), m_billDialog->week());
        m_weeklyTimesheetDialog->show();
        break;
    case BillDialog::Later:
        break;
    }
    if (CONFIGURATION.warnUnuploadedTimesheets)
        m_checkUploadedSheetsTimer.start();
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
                Event old = event;
                QDateTime start  = period.first; // initializes a valid QDateTime
                event.setEndDateTime( qMax( event.startDateTime(), start ) );
                Q_ASSERT( event.isValid() );
                CommandModifyEvent* cmd = new CommandModifyEvent( event, old, this );
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

static void setValueIfNotNull(QSettings* s, const QString& key, const QString& value )
{
    if ( !value.isNull() )
        s->setValue( key, value );
    else
        s->remove( key );
}

void TimeTrackingWindow::importTasksFromDeviceOrFile( QIODevice* device, const QString& filename )
{
    const MakeTemporarilyVisible m( this );
    Q_UNUSED( m );

    TaskExport exporter;
    TaskListMerger merger;
    try {
        if ( device )
            exporter.readFrom( device );
        else
            exporter.readFrom( filename );
        merger.setOldTasks( DATAMODEL->getAllTasks() );
        merger.setNewTasks( exporter.tasks() );
        if ( merger.modifiedTasks().isEmpty() && merger.addedTasks().isEmpty() ) {
            QMessageBox::information( this, tr( "Tasks Import" ), tr( "The selected task file does not contain any updates." ) );
        } else {
            CommandSetAllTasks* cmd = new CommandSetAllTasks( merger.mergedTaskList(), this );
            sendCommand( cmd );
            const QString detailsText =
                tr( "Task file imported, %1 tasks have been modified and %2 tasks added." )
                .arg( QString::number( merger.modifiedTasks().count() ),
                      QString::number( merger.addedTasks().count() ) );
            QMessageBox::information( this, tr( "Tasks Import" ), detailsText );
        }
        QSettings settings;
        settings.beginGroup( "httpconfig" );
        setValueIfNotNull( &settings, QLatin1String("username"), exporter.metadata( QLatin1String("username") ) );
        setValueIfNotNull( &settings, QLatin1String("portalUrl"), exporter.metadata( QLatin1String("portal-url") ) );
        setValueIfNotNull( &settings, QLatin1String("loginUrl"), exporter.metadata( QLatin1String("login-url") ) );
        setValueIfNotNull( &settings, QLatin1String("timesheetUploadUrl"), exporter.metadata( QLatin1String("timesheet-upload-url") ) );
        setValueIfNotNull( &settings, QLatin1String("projectCodeDownloadUrl"), exporter.metadata( QLatin1String("project-code-download-url") ) );
        Application::instance().setHttpActionsVisible( true );
    } catch( const CharmException& e ) {
        const QString message = e.what().isEmpty()
                                ?  tr( "The selected task definitions are invalid and cannot be imported." )
                                    : tr( "There was an error importing the task definitions:<br />%1" ).arg( e.what() );
        QMessageBox::critical( this, tr( "Invalid Task Definitions" ), message );
        return;
    }
}

#include "TimeTrackingWindow.moc"
