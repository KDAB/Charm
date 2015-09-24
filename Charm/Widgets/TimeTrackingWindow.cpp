/*
  TimeTrackingWindow.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Frank Osterfeld <frank.osterfeld@kdab.com>
  Author: Mathias Hasselmann <mathias.hasselmann@kdab.com>

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

#include "TimeTrackingWindow.h"
#include "ActivityReport.h"
#include "ApplicationCore.h"
#include "CharmAboutDialog.h"
#include "CharmCMake.h"
#include "CharmNewReleaseDialog.h"
#include "CharmPreferences.h"
#include "CommentEditorPopup.h"
#include "EnterVacationDialog.h"
#include "IdleCorrectionDialog.h"
#include "MakeTemporarilyVisible.h"
#include "MessageBox.h"
#include "MonthlyTimesheet.h"
#include "MonthlyTimesheetConfigurationDialog.h"
#include "TimeTrackingView.h"
#include "Uniquifier.h"
#include "ViewHelpers.h"
#include "WeeklyTimesheet.h"

#include "Commands/CommandExportToXml.h"
#include "Commands/CommandImportFromXml.h"
#include "Commands/CommandMakeEvent.h"
#include "Commands/CommandModifyEvent.h"
#include "Commands/CommandSetAllTasks.h"

#include "Core/TaskListMerger.h"
#include "Core/TimeSpans.h"
#include "Core/XmlSerialization.h"

#include "HttpClient/GetProjectCodesJob.h"
#include "HttpClient/GetUserInfoJob.h"

#include "Idle/IdleDetector.h"

#include "Widgets/HttpJobProgressDialog.h"

#include <QBuffer>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <QToolBar>
#include <QtAlgorithms>

#include <algorithm>

TimeTrackingWindow::TimeTrackingWindow( QWidget* parent )
    : CharmWindow( tr( "Time Tracker" ), parent )
    , m_weeklyTimesheetDialog( nullptr )
    , m_monthlyTimesheetDialog( nullptr )
    , m_activityReportDialog( nullptr )
    , m_summaryWidget( new TimeTrackingView( this ) )
    , m_billDialog( new BillDialog( this ) )
{
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    setWindowNumber( 3 );
    setWindowIdentifier( QLatin1String( "window_tracking" ) );
    setCentralWidget( m_summaryWidget );
    connect( m_summaryWidget, SIGNAL(startEvent(TaskId)),
             SLOT(slotStartEvent(TaskId)) );
    connect( m_summaryWidget, SIGNAL(stopEvents()),
             SLOT(slotStopEvent()) );
    connect( &m_checkUploadedSheetsTimer, SIGNAL(timeout()),
             SLOT(slotCheckUploadedTimesheets()) );
    connect( m_billDialog, SIGNAL(finished(int)),
             SLOT(slotBillGone(int)) );
    connect( &m_checkCharmReleaseVersionTimer, SIGNAL(timeout()),
             SLOT(slotCheckForUpdatesAutomatic()) );
    connect( &m_updateTasksDefinitionsTimer, SIGNAL(timeout()),
             SLOT(slotSyncTasksAutomatic()) );

    //Check every 60 minutes if there are timesheets due
    if (CONFIGURATION.warnUnuploadedTimesheets)
        m_checkUploadedSheetsTimer.start();
    m_checkUploadedSheetsTimer.setInterval(60 * 60 * 1000);
#if defined(Q_OS_OSX) || defined(Q_OS_WIN)
    m_checkCharmReleaseVersionTimer.setInterval(24 * 60 * 60 * 1000);
    if ( !QString::fromLatin1(UPDATE_CHECK_URL).isEmpty() ) {
        QTimer::singleShot( 1000, this, SLOT(slotCheckForUpdatesAutomatic()) );
        m_checkCharmReleaseVersionTimer.start();
    }
#endif
    //Update tasks definitions once every 24h
    m_updateTasksDefinitionsTimer.setInterval(24 * 60 * 60 * 1000);
    QTimer::singleShot( 1000, this, SLOT(slotSyncTasksAutomatic()) );
    m_updateTasksDefinitionsTimer.start();

    toolBar()->hide();
}

bool TimeTrackingWindow::event( QEvent* event )
{
    if (event->type() == QEvent::LayoutRequest)
        setMaximumHeight( sizeHint().height() );
    return CharmWindow::event( event );
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
    if ( ApplicationCore::hasInstance() ) {
        DATAMODEL->unregisterAdapter( this );
    }
}

void TimeTrackingWindow::stateChanged( State previous )
{
    CharmWindow::stateChanged( previous );
    switch( ApplicationCore::instance().state() ) {
    case Connecting: {
        connect( ApplicationCore::instance().dateChangeWatcher(), SIGNAL(dateChanged()),
                 SLOT(slotSelectTasksToShow()) );
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

    if (CONFIGURATION.requestEventComment) {
        Event event = DATAMODEL->eventForId( id );
        if ( event.isValid() && event.comment().isEmpty() ) {
            CommentEditorPopup popup;
            popup.loadEvent( id );
            popup.exec();
        }
    }
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
    } else {
        QString nm = item.task().name();
        QMessageBox::critical( this, tr( "Invalid task" ),
                               tr( "Task '%1' is no longer valid, so can't be started" ).arg( nm ) );
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
        CONFIGURATION.requestEventComment = dialog.requestEventComment();
        CONFIGURATION.enableCommandInterface = dialog.enableCommandInterface();
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
        auto command = new CommandMakeEvent( event, this );
        sendCommand( command );
    }
}

void TimeTrackingWindow::slotActivityReport()
{
    delete m_activityReportDialog;
    m_activityReportDialog = new ActivityReportConfigurationDialog( this );
    m_activityReportDialog->setAttribute( Qt::WA_DeleteOnClose );
    connect( m_activityReportDialog, SIGNAL(finished(int)),
             this, SLOT(slotActivityReportPreview(int)) );
    m_activityReportDialog->show();
}

void TimeTrackingWindow::resetWeeklyTimesheetDialog()
{
    delete m_weeklyTimesheetDialog;
    m_weeklyTimesheetDialog = new WeeklyTimesheetConfigurationDialog( this );
    m_weeklyTimesheetDialog->setAttribute( Qt::WA_DeleteOnClose );
    connect( m_weeklyTimesheetDialog, SIGNAL(finished(int)),
             this, SLOT(slotWeeklyTimesheetPreview(int)) );
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
    connect( m_monthlyTimesheetDialog, SIGNAL(finished(int)),
             this, SLOT(slotMonthlyTimesheetPreview(int)) );
}

void TimeTrackingWindow::slotMonthlyTimesheetReport()
{
    resetMonthlyTimesheetDialog();
    m_monthlyTimesheetDialog->show();
}

void TimeTrackingWindow::slotWeeklyTimesheetPreview( int result )
{
    showPreview( m_weeklyTimesheetDialog, result );
    m_weeklyTimesheetDialog = nullptr;
}

void TimeTrackingWindow::slotMonthlyTimesheetPreview( int result )
{
    showPreview( m_monthlyTimesheetDialog, result );
    m_monthlyTimesheetDialog = nullptr;
}

void TimeTrackingWindow::slotActivityReportPreview( int result )
{
    showPreview( m_activityReportDialog, result );
    m_activityReportDialog = nullptr;
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

void TimeTrackingWindow::slotSyncTasks( VerboseMode mode )
{
    GetProjectCodesJob* client = new GetProjectCodesJob( this );
    if ( mode == Verbose ) {
        HttpJobProgressDialog* dialog = new HttpJobProgressDialog( client, this );
        dialog->setWindowTitle( tr("Downloading") );
    } else
        client->setVerbose( false );

    connect( client, SIGNAL(finished(HttpJob*)), this, SLOT(slotTasksDownloaded(HttpJob*)) );
    client->start();
}

void TimeTrackingWindow::slotSyncTasksAutomatic()
{
    // check if HttpJob is possible
    if ( HttpJob::credentialsAvailable() && !HttpJob::lastAuthenticationFailed() ) {
        slotSyncTasks( Silent );
    }
}

void TimeTrackingWindow::slotTasksDownloaded(HttpJob* job_)
{
    GetProjectCodesJob* job = qobject_cast<GetProjectCodesJob*>( job_ );
    Q_ASSERT( job );
    const bool verbose = job->isVerbose();
    if ( job->error() == HttpJob::Canceled )
        return;

    if ( job->error() ) {
        const QString title = tr("Error");
        const QString message = tr("Could not download the task list: %1").arg( job->errorString() );
        if ( verbose )
            QMessageBox::critical( this, title, message );
        else
            emit showNotification( title, message );

        return;
    }

    QBuffer buffer;
    buffer.setData( job->payload() );
    buffer.open( QIODevice::ReadOnly );
    importTasksFromDeviceOrFile( &buffer, QString(), verbose );
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

void TimeTrackingWindow::slotCheckForUpdatesAutomatic()
{
    // do not display message error when auto-checking
    startCheckForUpdates();
}

void TimeTrackingWindow::slotCheckForUpdatesManual()
{
    startCheckForUpdates( Verbose );
}

void TimeTrackingWindow::startCheckForUpdates( VerboseMode mode )
{
    CheckForUpdatesJob* checkForUpdates = new CheckForUpdatesJob( this );
    connect( checkForUpdates, SIGNAL(finished(CheckForUpdatesJob::JobData)), this, SLOT(slotCheckForUpdates(CheckForUpdatesJob::JobData)) );
    const QString urlString = UPDATE_CHECK_URL;
    checkForUpdates->setUrl( QUrl( urlString ) );
    if ( mode == Verbose )
        checkForUpdates->setVerbose( true );
    checkForUpdates->start();
}

void TimeTrackingWindow::slotCheckForUpdates( CheckForUpdatesJob::JobData data )
{
    const QString errorString = data.errorString;
    if ( data.verbose && ( data.error != 0 || !errorString.isEmpty() ) ) {
        QMessageBox::critical( this, tr( "Error" ), errorString );
        return;
    }

    const QString releaseVersion = data.releaseVersion;

    QSettings settings;
    settings.beginGroup( QLatin1String( "UpdateChecker" ) );
    const QString skipVersion = settings.value( QLatin1String( "skip-version" ) ).toString();
    if ( ( skipVersion == releaseVersion ) && !data.verbose )
        return;

    if ( Charm::versionLessThan( CHARM_VERSION, releaseVersion ) ) {
        informUserAboutNewRelease( releaseVersion, data.link, data.releaseInformationLink );
    } else {
        if ( data.verbose )
            QMessageBox::information( this, tr( "Charm Release" ), tr( "There is no newer Charm version available!" ) );
    }
}

void TimeTrackingWindow::informUserAboutNewRelease( const QString& releaseVersion, const QUrl& link, const QString& releaseInfoLink )
{
    QString localVersion = CHARM_VERSION;
    localVersion.truncate( releaseVersion.length() );
    CharmNewReleaseDialog dialog( this );
    dialog.setVersion( releaseVersion, localVersion );
    dialog.setDownloadLink( link );
    dialog.setReleaseInformationLink( releaseInfoLink );
    dialog.exec();
}

void TimeTrackingWindow::maybeIdle( IdleDetector* detector )
{
    Q_ASSERT( detector );
    static bool inProgress = false;

    if ( inProgress == true ) return;
    Uniquifier u( &inProgress );

    Q_FOREACH( const IdleDetector::IdlePeriod& p, detector->idlePeriods() ) {
        qDebug() << "ApplicationCore::slotMaybeIdle: computer might be have been idle from"
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
                auto cmd = new CommandModifyEvent( event, old, this );
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

void TimeTrackingWindow::importTasksFromDeviceOrFile( QIODevice* device, const QString& filename , bool verbose )
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
            const QString title = tr( "Tasks Import" );
            const QString message = tr( "The selected task file does not contain any updates." );
            if ( verbose )
                QMessageBox::information( this, title, message );
            else
                emit showNotification( title, message );
        } else {
            auto cmd = new CommandSetAllTasks( merger.mergedTaskList(), this );
            sendCommand( cmd );
            // At this point the command was finalized and we have a result.
            const bool success = cmd->finalize();
            const QString detailsText = success ? tr( "The task list has been updated." ) : tr( "Setting the new tasks failed." );
            const QString title = success ? tr( "Tasks Import" ) : tr( "Failure setting new tasks" );
            if ( verbose )
                QMessageBox::information( this, title, detailsText );
            else
                emit showNotification( title, detailsText );
            getUserInfo();
        }

        QSettings settings;
        settings.beginGroup( "httpconfig" );
        setValueIfNotNull( &settings, QLatin1String("username"), exporter.metadata( QLatin1String("username") ) );
        setValueIfNotNull( &settings, QLatin1String("portalUrl"), exporter.metadata( QLatin1String("portal-url") ) );
        setValueIfNotNull( &settings, QLatin1String("loginUrl"), exporter.metadata( QLatin1String("login-url") ) );
        setValueIfNotNull( &settings, QLatin1String("timesheetUploadUrl"), exporter.metadata( QLatin1String("timesheet-upload-url") ) );
        setValueIfNotNull( &settings, QLatin1String("projectCodeDownloadUrl"), exporter.metadata( QLatin1String("project-code-download-url") ) );
        settings.endGroup();
        settings.beginGroup( "users" );
        settings.setValue( QLatin1String("portalUrl"), QLatin1String("https://lotsofcake.kdab.com:443/KdabHome/apps/portal/"));
        settings.setValue( QLatin1String("loginUrl"), QLatin1String("https://lotsofcake.kdab.com:443/KdabHome/apps/portal/j_security_check"));
        settings.setValue( QLatin1String("userInfoDownloadUrl"), QLatin1String("https://lotsofcake.kdab.com/KdabHome/rest/user"));
        settings.endGroup();

        ApplicationCore::instance().setHttpActionsVisible( true );
    } catch( const CharmException& e ) {
        const QString title = tr( "Invalid Task Definitions" );
        const QString message = e.what().isEmpty()
                                ?  tr( "The selected task definitions are invalid and cannot be imported." )
                                    : tr( "There was an error importing the task definitions:<br />%1" ).arg( e.what() );
        if ( verbose )
            QMessageBox::critical( this, title, message );
        else
            emit showNotification( title, message );
        return;
    }
}

void  TimeTrackingWindow::getUserInfo()
{
    if (!HttpJob::credentialsAvailable())
            return;

    QSettings settings;
    settings.beginGroup( "httpconfig" );
    m_user = settings.value("username").toString();
    settings.endGroup();

    settings.beginGroup( "users" );
    settings.setValue( QLatin1String("userInfoDownloadUrl"), QLatin1String("https://lotsofcake.kdab.com/KdabHome/rest/user?user=") + m_user );
    settings.endGroup();

    GetUserInfoJob *client = new GetUserInfoJob(this,"users");
    client->schema() = m_user;
    HttpJobProgressDialog* dialog = new HttpJobProgressDialog( client, this );
    dialog->setWindowTitle( tr("Downloading weekly hours") );
    connect( client, SIGNAL(finished(HttpJob*)), this, SLOT(slotUserInfoDownloaded(HttpJob*)) );
    client->start();
}

void TimeTrackingWindow::slotUserInfoDownloaded( HttpJob* job_ )
{
    GetUserInfoJob * job = qobject_cast<GetUserInfoJob *>( job_ );
    Q_ASSERT( job );
    if ( job->error() == HttpJob::Canceled )
        return;

    if ( job->error() ) {
        QMessageBox::critical( this, tr("Error"), tr("Could not download weekly hours: %1").arg( job->errorString() ) );
        return;
    }

    QByteArray readData = job->userInfo();
    int index = readData.indexOf("weeklyHours");
    index += 13;
    QString weeklyH = readData.mid(index,2).trimmed();
    if (weeklyH.length() != 2 )
        weeklyH = "40";
    QSettings settings;
    settings.beginGroup( "users" );
    settings.setValue( QLatin1String("weeklyhours"), weeklyH);
    settings.endGroup();

}
#include "moc_TimeTrackingWindow.cpp"
