#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QSettings>
#include <QMenuBar>

#include <Core/CharmConstants.h>
#include <Core/CharmExceptions.h>
#include <Core/TaskListMerger.h>
#include <Core/XmlSerialization.h>

#include "Application.h"
#include "ViewHelpers.h"
#include "TasksWindow.h"
#include "TasksView.h"
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

TasksWindow::TasksWindow( QWidget* parent )
    : CharmWindow( tr( "Tasks" ), parent )
    , m_tasksView( new TasksView( toolBar(), this ) )
    , m_reportDialog( this )
{
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::MinimumExpanding );
    setWindowNumber( 1 );
    setWindowIdentifier( QLatin1String( "window_tasks" ) );
    setCentralWidget( m_tasksView );
    setUnifiedTitleAndToolBarOnMac( true );
    connect( m_tasksView, SIGNAL( emitCommand( CharmCommand* ) ),
             SIGNAL( emitCommand( CharmCommand* ) ) );
    connect( m_tasksView, SIGNAL( saveConfiguration() ),
             SIGNAL( saveConfiguration() ) );
}

TasksWindow::~TasksWindow()
{
}

void TasksWindow::stateChanged( State previous )
{
    CharmWindow::stateChanged( previous );
    m_tasksView->stateChanged( previous );

    if ( Application::instance().state() == Connecting ) {
        m_tasksView->setModel( & Application::instance().model() );
    }
}

void TasksWindow::restore()
{
    show();
}

void TasksWindow::configurationChanged()
{
    CharmWindow::configurationChanged();
    m_tasksView->configurationChanged();
}

void TasksWindow::insertEditMenu()
{
    QMenu* editMenu = menuBar()->addMenu( tr( "Edit" ) );
    m_tasksView->populateEditMenu( editMenu);
}

void TasksWindow::slotEditPreferences( bool )
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
        emit saveConfiguration();
    }
}

void TasksWindow::slotAboutDialog()
{
    MakeTemporarilyVisible m( this );
    CharmAboutDialog dialog( this );
    dialog.exec();
}

void TasksWindow::slotReportDialog()
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

void TasksWindow::slotExportToXml()
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

void TasksWindow::slotImportFromXml()
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

void TasksWindow::slotImportTasks()
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

void TasksWindow::maybeIdle()
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

#include "TasksWindow.moc"
