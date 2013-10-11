/*
 *  Application.cpp
 *  Charm
 *
 *  Created by Mirko Boehm on 9/15/06.
 *  Copyright 2006 KDAB. All rights reserved.
 *
 */

#include <QDir>
#include <QTimer>
#include <QAction>
#include <QSettings>
#include <QMetaType>
#include <QMenuBar>
#include <QMessageBox>
#include <QSessionManager>
#include <QLocalSocket>
#include <QFile>

#if QT_VERSION < 0x050000
#include <QDesktopServices>
#else
#include <QStandardPaths>
#endif

#include <Core/CharmConstants.h>
#include <Core/CharmExceptions.h>
#include <Core/SqLiteStorage.h>
#include <CharmCMake.h>

#include "ViewHelpers.h"
#include "Data.h"
#include "Application.h"
#include "ConfigurationDialog.h"
#include "Idle/IdleDetector.h"
#include "Uniquifier.h"
#include "HttpClient/HttpJob.h"

#include <algorithm> //for_each()

Application* Application::m_instance = 0;

Application::Application(int& argc, char** argv)
    : QApplication( argc, argv )
    , m_closedWindow( 0 )
    , m_actionStopAllTasks( this )
    , m_windows( QList<CharmWindow*> () << &m_tasksWindow << &m_eventWindow << &m_timeTracker )
    , m_actionQuit( this )
    , m_state(Constructed)
    , m_actionAboutDialog( this )
    , m_actionPreferences( this )
    , m_actionExportToXml( this )
    , m_actionImportFromXml( this )
    , m_actionSyncTasks( this )
    , m_actionImportTasks( this )
    , m_actionExportTasks( this )
    , m_actionEnterVacation( this )
    , m_actionActivityReport( this )
    , m_actionWeeklyTimesheetReport( this )
    , m_actionMonthlyTimesheetReport( this )
    , m_idleDetector( 0 )
    , m_timeTrackerHiddenFromSystrayToggle( false )
    , m_tasksWindowHiddenFromSystrayToggle( false )
    , m_eventWindowHiddenFromSystrayToggle( false )
    , m_dateChangeWatcher( new DateChangeWatcher( this ) )
{
    // QApplication setup
    setQuitOnLastWindowClosed(false);
    // application metadata setup
    // note that this modifies the behaviour of QSettings:
    QCoreApplication::setOrganizationName("KDAB");
    QCoreApplication::setOrganizationDomain("kdab.com");
    QCoreApplication::setApplicationName("Charm");
    QCoreApplication::setApplicationVersion(CHARM_VERSION);

    QLocalSocket uniqueApplicationSocket;
    QString serverName( "com.kdab.charm" );
#ifndef NDEBUG
    serverName.append( "_debug" );
#endif
    uniqueApplicationSocket.connectToServer(serverName, QIODevice::ReadOnly);
    if (uniqueApplicationSocket.waitForConnected(1000))
        throw AlreadyRunningException();

    connect(&m_uniqueApplicationServer, SIGNAL(newConnection()),
            this, SLOT(slotHandleUniqueApplicationConnection()));

    QFile::remove(QDir::tempPath() + '/' + serverName);
    bool listening = m_uniqueApplicationServer.listen(serverName);
    if (!listening)
        qDebug() << "Failed to create QLocalServer for unique application support:"
                 << m_uniqueApplicationServer.errorString();

    Q_INIT_RESOURCE(CharmResources);
    Q_ASSERT_X(m_instance == 0, "Application ctor",
               "Application is a singleton and cannot be created more than once");
    m_instance = this;
    qRegisterMetaType<State> ("State");
    qRegisterMetaType<Event> ("Event");

    // exit process (app will only exit once controller says it is ready)
    connect(&m_controller, SIGNAL(readyToQuit()), SLOT(
                slotControllerReadyToQuit()));

    connectControllerAndModel(&m_controller, m_model.charmDataModel());
    connectControllerAndView(&m_controller, &mainView());
    Q_FOREACH( CharmWindow* window, m_windows ) {
        if ( window != &mainView() ) { // main view acts as the main relay
            connect( window, SIGNAL( emitCommand( CharmCommand* ) ),
                     &mainView(), SLOT( sendCommand( CharmCommand* ) ) );
            connect( window, SIGNAL( emitCommandRollback( CharmCommand* ) ),
                     &mainView(), SLOT( sendCommandRollback( CharmCommand* ) ) );
        }
        // save the configuration (configuration is managed by the application)
        connect( window, SIGNAL(saveConfiguration() ),
                 SLOT( slotSaveConfiguration() ) );

        connect( window, SIGNAL( visibilityChanged( bool ) ),
                 this,   SLOT( slotCharmWindowVisibilityChanged( bool ) ) );
    }
    // my own signals:
    connect(this, SIGNAL(goToState(State)), SLOT(setState(State)),
            Qt::QueuedConnection);

    // system tray icon:
    m_actionStopAllTasks.setText( tr( "Stop &All Active Tasks" ) );
    m_actionStopAllTasks.setShortcut( Qt::Key_Escape );
    m_actionStopAllTasks.setShortcutContext( Qt::ApplicationShortcut );
    mainView().addAction(&m_actionStopAllTasks); // for the shortcut to work
    connect( &m_actionStopAllTasks, SIGNAL( triggered() ), SLOT( slotStopAllTasks() ) );

    m_systrayContextMenu.addAction( &m_actionStopAllTasks );
    m_systrayContextMenu.addSeparator();

    m_trayIcon.setContextMenu( &m_systrayContextMenu );
    m_trayIcon.setToolTip( tr( "No active events" ) );
    m_trayIcon.setIcon( Data::charmTrayIcon() );
    m_trayIcon.show();

    setWindowIcon( Data::charmIcon() );

    Q_FOREACH( CharmWindow* window, m_windows )
        m_systrayContextMenu.addAction( window->showHideAction() );

    m_systrayContextMenu.addSeparator();
    m_systrayContextMenu.addMenu( m_timeTracker.menu() );

    m_systrayContextMenu.addSeparator();
    m_systrayContextMenu.addAction( &m_actionQuit );

    // set up actions:
    m_actionQuit.setShortcut( Qt::CTRL + Qt::Key_Q );
    m_actionQuit.setText( tr( "Quit" ) );
    m_actionQuit.setIcon( Data::quitCharmIcon() );
    connect( &m_actionQuit, SIGNAL( triggered( bool ) ),
             SLOT( slotQuitApplication() ) );

    m_actionAboutDialog.setText( tr( "About Charm" ) );
    connect( &m_actionAboutDialog, SIGNAL( triggered() ),
             &mainView(),  SLOT( slotAboutDialog() ) );

    m_actionPreferences.setText( tr( "Preferences" ) );
    m_actionPreferences.setIcon( Data::configureIcon() );
    connect( &m_actionPreferences, SIGNAL( triggered( bool ) ),
             &mainView(),  SLOT( slotEditPreferences( bool ) ) );
    m_actionPreferences.setEnabled( true );

    m_actionImportFromXml.setText( tr( "Import Database from Previous Export..." ) );
    connect( &m_actionImportFromXml, SIGNAL( triggered() ),
             &mainView(),  SLOT( slotImportFromXml() ) );
    m_actionExportToXml.setText( tr( "Export Database..." ) );
    connect( &m_actionExportToXml, SIGNAL( triggered() ),
             &mainView(),  SLOT( slotExportToXml() ) );
    m_actionSyncTasks.setText( tr( "Download Task Definitions..." ) );
    connect( &m_actionSyncTasks, SIGNAL( triggered() ),
             &mainView(),  SLOT( slotSyncTasks() ) );
    m_actionImportTasks.setText( tr( "Import and Merge Task Definitions..." ) );
    connect( &m_actionImportTasks, SIGNAL( triggered() ),
             &mainView(),  SLOT( slotImportTasks() ) );
    m_actionExportTasks.setText( tr( "Export Task Definitions..." ) );
    connect( &m_actionExportTasks, SIGNAL( triggered() ),
             &mainView(), SLOT( slotExportTasks() ) );
    m_actionEnterVacation.setText( tr( "Enter Vacation...") );
    connect( &m_actionEnterVacation, SIGNAL(triggered()),
             &mainView(), SLOT(slotEnterVacation()) );
    m_actionActivityReport.setText( tr( "Activity Report..." ) );
    m_actionActivityReport.setShortcut( Qt::CTRL + Qt::Key_A );
    connect( &m_actionActivityReport, SIGNAL( triggered() ),
             &mainView(), SLOT( slotActivityReport() ) );
    m_actionWeeklyTimesheetReport.setText( tr( "Weekly Timesheet...") );
    m_actionWeeklyTimesheetReport.setShortcut( Qt::CTRL + Qt::Key_R );
    connect( &m_actionWeeklyTimesheetReport, SIGNAL( triggered() ),
             &mainView(), SLOT( slotWeeklyTimesheetReport() ) );
    m_actionMonthlyTimesheetReport.setText( tr( "Monthly Timesheet...") );
    m_actionMonthlyTimesheetReport.setShortcut( Qt::CTRL + Qt::Key_M );
    connect( &m_actionMonthlyTimesheetReport, SIGNAL( triggered() ),
             &mainView(), SLOT( slotMonthlyTimesheetReport() ) );

    // set up idle detection
    m_idleDetector = IdleDetector::createIdleDetector( this );
    if ( m_idleDetector == 0 ) {
        qDebug() << "Application ctor: idle detection is not available on this platform.";
    } else {
        qDebug() << "Application ctor: idle detection initialized.";
        connect( m_idleDetector, SIGNAL( maybeIdle() ), SLOT( slotMaybeIdle() ) );
    }

    setHttpActionsVisible(HttpJob::credentialsAvailable());
    // add default plugin path for deployment
    addLibraryPath( applicationDirPath() + "/plugins" );

    if ( applicationDirPath().endsWith("MacOS") )
        addLibraryPath( applicationDirPath() + "/../plugins");

    // Ladies and gentlemen, please raise upon your seats -
    // the show is about to begin:
    emit goToState(StartingUp);
}

Application::~Application()
{
}

void Application::slotHandleUniqueApplicationConnection()
{
    QLocalSocket* socket = m_uniqueApplicationServer.nextPendingConnection();
    delete socket;
    openAWindow( true );
}

void Application::openAWindow( bool raise ) {
    CharmWindow* windowToOpen = 0;
    foreach( CharmWindow* window, m_windows )
        if ( !window->isHidden() )
            windowToOpen = window;

    if ( !windowToOpen && m_closedWindow )
        windowToOpen = m_closedWindow;

    if ( !windowToOpen )
        windowToOpen = &mainView();

    windowToOpen->show();
    if ( raise )
        windowToOpen->raise();

    if( windowToOpen == m_closedWindow )
        m_closedWindow = 0;
}

void Application::createWindowMenu( QMenuBar *menuBar )
{
    QMenu* menu = new QMenu( menuBar );
    menu->setTitle( tr( "Window" ) );
    Q_FOREACH( CharmWindow* window, m_windows ) {
        menu->addAction( window->showHideAction() );
    }
    menu->addSeparator();
    menu->addAction( &m_actionEnterVacation );
    menu->addSeparator();
    menu->addAction( &m_actionActivityReport );
    menu->addAction( &m_actionWeeklyTimesheetReport );
    menu->addAction( &m_actionMonthlyTimesheetReport );
    menu->addSeparator();
    menu->addAction( &m_actionPreferences );
    menuBar->addMenu( menu );
}

void Application::createFileMenu( QMenuBar *menuBar )
{
    QMenu* menu = new QMenu( menuBar );
    menu->setTitle ( tr( "File" ) );
    menu->addAction( &m_actionImportFromXml );
    menu->addAction( &m_actionExportToXml );
    menu->addSeparator();
    menu->addAction( &m_actionSyncTasks );
    menu->addAction( &m_actionImportTasks );
    menu->addAction( &m_actionExportTasks );
    menu->addSeparator();
    menu->addAction( &m_actionQuit );
    menuBar->addMenu( menu );
}

void Application::createHelpMenu( QMenuBar *menuBar )
{
    QMenu* menu = new QMenu( menuBar );
    menu->setTitle( tr( "Help" ) );
    menu->addAction( &m_actionAboutDialog );
    menuBar->addMenu( menu );
}

void Application::setState(State state)
{
    if (m_state == state)
        return;
    qDebug() << "Application::setState: going from" << StateNames[m_state]
             << "to" << StateNames[state];
    State previous = m_state;

    try {

    switch (m_state)
    {
    case Constructed:
        break; // ignore, but this state is never re-entered
    case StartingUp:
        leaveStartingUpState();
        break;
    case Configuring:
        leaveConfiguringState();
        break;
    case Connecting:
        leaveConnectingState();
        break;
    case Connected:
        leaveConnectedState();
        break;
    case Disconnecting:
        leaveDisconnectingState();
        break;
    case ShuttingDown:
        leaveShuttingDownState();
        break;
    default:
        Q_ASSERT_X(false, "Application::setState",
                   "Unknown previous application state");
    };

    m_state = state;

    std::for_each( m_windows.begin(), m_windows.end(),
                   std::bind2nd( std::mem_fun( &CharmWindow::stateChanged ), m_state ) );

    switch (m_state)
    {
    case StartingUp:
        m_model.charmDataModel()->stateChanged(previous, state);
        m_controller.stateChanged(previous, state);
        // FIXME unnecessary?
        // m_mainWindow.stateChanged(previous);
        // m_timeTracker.stateChanged( previous );
        enterStartingUpState();
        break;
    case Configuring:
        enterConfiguringState();
        break;
    case Connecting:
        m_model.charmDataModel()->stateChanged(previous, state);
        m_controller.stateChanged(previous, state);
        // FIXME unnecessary?
        // m_mainWindow.stateChanged(previous);
        // m_timeTracker.stateChanged( previous );
        enterConnectingState();
        break;
    case Connected:
        m_model.charmDataModel()->stateChanged(previous, state);
        m_controller.stateChanged(previous, state);
        // FIXME unnecessary?
        // m_mainWindow.stateChanged(previous);
        // m_timeTracker.stateChanged( previous );
        enterConnectedState();
        break;
    case Disconnecting:
        // FIXME unnecessary?
        // m_timeTracker.stateChanged( previous );
        // m_mainWindow.stateChanged(previous);
        m_model.charmDataModel()->stateChanged(previous, state);
        m_controller.stateChanged(previous, state);
        enterDisconnectingState();
        break;
    case ShuttingDown:
        // FIXME unnecessary?
        // m_timeTracker.stateChanged( previous );
        // m_mainWindow.stateChanged(previous);
        m_model.charmDataModel()->stateChanged(previous, state);
        m_controller.stateChanged(previous, state);
        enterShuttingDownState();
        break;
    default:
        Q_ASSERT_X(false, "Application::setState",
                   "Unknown new application state");
    };
    } catch( const CharmException& e ) {
        QMessageBox::critical( &mainView(), tr( "Critical Charm Problem" ),
                               e.what() );
        quit();
    }
}

State Application::state() const
{
    return m_state;
}

Application& Application::instance()
{
    Q_ASSERT_X(m_instance, "Application::instance",
               "Singleton not constructed yet");
    return *m_instance;
}

bool Application::hasInstance()
{
    return m_instance != 0;
}

void Application::enterStartingUpState()
{
    emit goToState( Configuring );
}

void Application::leaveStartingUpState()
{
}

void Application::enterConfiguringState()
{

    if (configure())
    { // if all ok, go to connecting state
        emit goToState(Connecting);
    } else {
        // user has cancelled configure, exit the application
        quit();
    }
}

void Application::leaveConfiguringState()
{
}

void Application::enterConnectingState()
{
    try {
        if (!m_controller.initializeBackEnd(CHARM_SQLITE_BACKEND_DESCRIPTOR))
            quit();
    } catch ( const CharmException& e ) {
        QMessageBox::critical( &mainView(), QObject::tr("Database Backend Error"),
                              tr( "The backend could not be initialized: %1" )
                              .arg( e.what() ) );
        slotQuitApplication();
        return;
    }
    // tell storage to connect to database
    CONFIGURATION.failure = false;
    try
    {
        if (m_controller.connectToBackend())
        {
            // delay switch to Connected state a bit to show the start screen:
            QTimer::singleShot(0, this, SLOT(slotGoToConnectedState()));
        }
        else
        {
            // go back to StartingUp state and reconfigure
            emit goToState(StartingUp);
        }
    } catch (const UnsupportedDatabaseVersionException& e) {
        qDebug() << e.what();
        QString message = QObject::tr( "<html><body>"
                                       "<p>Your current Charm database is too old to use with this version. You have two "
                                       "options here:</p><ul>"
                                       "<li>Start over with an empty database by moving or deleting your ~/.Charm folder "
                                       "then re-running this version of Charm.</li>"
                                       "<li>Load an older version of Charm that supports your current database and select "
                                       "File->Export, and save that file somewhere. Then, either rename or delete your "
                                       "~/.Charm folder and restart this version of Charm and select File->Import from "
                                       "previous export and select the file you saved in the previous step.</li>"
                                       "</ul></body></html>");
        QMessageBox::critical( &mainView(), QObject::tr("Charm Database Error"),
                              message);
        slotQuitApplication();
        return;
    }
}

void Application::leaveConnectingState()
{
}

void Application::enterConnectedState()
{
}

void Application::leaveConnectedState()
{
    m_controller.persistMetaData(CONFIGURATION);
}

void Application::enterDisconnectingState()
{
    // just wait for controller to emit readyToQuit()
}

void Application::leaveDisconnectingState()
{
}

void Application::enterShuttingDownState()
{
    QTimer::singleShot(0, this, SLOT(quit()));
}

void Application::leaveShuttingDownState()
{
}

void Application::slotGoToConnectedState()
{
    if (state() == Connecting)
    {
        emit goToState(Connected);
    }
}

static QString charmDataDir() {
    const QByteArray charmHome = qgetenv("CHARM_HOME");
    if ( !charmHome.isEmpty() )
        return QFile::decodeName( charmHome ) + QLatin1String("/data/");
#if QT_VERSION < 0x050000
    return QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QLatin1Char('/');
#else
    return QStandardPaths::writableLocation(QStandardPaths::DataLocation) + QLatin1Char('/');
#endif
}

bool Application::configure()
{
    if (CONFIGURATION.failure == true)
    {
        qDebug()
            << "Application::configure: an error was found within the configuration.";
        if (!CONFIGURATION.failureMessage.isEmpty())
        {
            QMessageBox::information( &mainView(),
                                     tr("Configuration Problem"), CONFIGURATION.failureMessage,
                                     tr("OK"));
            CONFIGURATION.failureMessage.clear();
        }
    }

    // load configuration:
    QSettings settings;
    settings.beginGroup(CONFIGURATION.configurationName);

    bool configurationComplete = CONFIGURATION.readFrom(settings);

    if (!configurationComplete || CONFIGURATION.failure)
    {
        qDebug()
            << "Application::configure: no complete configuration found for configuration name"
            << CONFIGURATION.configurationName;
        // FIXME maybe move to Configuration::loadDefaults

        const QString storageDatabaseDirectory = charmDataDir();
        const QString storageDatabaseFileRelease = "Charm.db";
        const QString storageDatabaseFileDebug = "Charm_debug.db";
        const QString storageDatabaseRelease = storageDatabaseDirectory + storageDatabaseFileRelease;
        const QString storageDatabaseDebug = storageDatabaseDirectory + storageDatabaseFileDebug;
        QString storageDatabase;
#ifdef NDEBUG
        Q_UNUSED( storageDatabaseDebug );
        storageDatabase = storageDatabaseRelease;
#else
        Q_UNUSED( storageDatabaseRelease );
        storageDatabase = storageDatabaseDebug;
#endif
        CONFIGURATION.localStorageDatabase = QDir::toNativeSeparators(storageDatabase);
        ConfigurationDialog dialog(CONFIGURATION, &mainView());
        if (dialog.exec())
        {
            CONFIGURATION = dialog.configuration();
            CONFIGURATION.writeTo(settings);
            mainView().show();
        }
        else
        {
            qDebug()
                << "Application::configure: user cancelled configuration. Exiting.";
            // quit();
            return false;
        }
    }

    return true;
}

void Application::toggleShowHide()
{
    if ( m_timeTracker.isHidden() && m_tasksWindow.isHidden() && m_eventWindow.isHidden() ) {
        int raised = 0;
        if ( m_eventWindowHiddenFromSystrayToggle ) {
            CharmWindow::showHideView( &m_eventWindow );
            m_eventWindowHiddenFromSystrayToggle = false;
            ++raised;
        }
        if ( m_tasksWindowHiddenFromSystrayToggle ) {
            CharmWindow::showHideView( &m_tasksWindow );
            m_tasksWindowHiddenFromSystrayToggle = false;
            ++raised;
        }
        if ( m_timeTrackerHiddenFromSystrayToggle || raised == 0 ) { // if no view was visible and the user did not toggle other views before, raise the timetracker
            m_timeTracker.showHideView();
            m_timeTrackerHiddenFromSystrayToggle = false;
        }
    } else {
        if ( m_timeTracker.isVisible() ) {
            m_timeTracker.hide();
            m_timeTrackerHiddenFromSystrayToggle = true;
        }
        if ( m_tasksWindow.isVisible() ) {
            m_tasksWindow.hide();
            m_tasksWindowHiddenFromSystrayToggle = true;
        }
        if ( m_eventWindow.isVisible() ) {
            m_eventWindow.hide();
            m_eventWindowHiddenFromSystrayToggle = true;
        }
    }
}

QString Application::titleString( const QString& text ) const
{
    QString dbInfo;
    const QString userName = CONFIGURATION.user.name();
    if ( !text.isEmpty() ) {
        if ( !userName.isEmpty()) {
            dbInfo = QString("%1 - %2").arg(userName, text);
        } else {
            dbInfo = text;
        }
        return tr( "Charm (%1)" ).arg( dbInfo );
    } else {
        return tr( "Charm" );
    }
}

void Application::slotCurrentBackendStatusChanged( const QString& text )
{
    const QString title = titleString( text );

    // FIXME why can't this be done on stateChanged()? and if not, is
    // maybe an app-wide metadataChanged() or configurationChanged()
    // missing? (the latter exists)
    // MIRKO_TEMP_REM
    /*
    m_mainWindow.setWindowTitle( title );
    */
    m_trayIcon.setToolTip( title );
}

void Application::slotStopAllTasks()
{
    DATAMODEL->endAllEventsRequested();
}

void Application::slotQuitApplication()
{
    emit goToState(Disconnecting);
}

void Application::slotControllerReadyToQuit()
{
    emit goToState(ShuttingDown);
}

void Application::slotSaveConfiguration()
{
    QSettings settings;
    settings.beginGroup(CONFIGURATION.configurationName);
    CONFIGURATION.writeTo(settings);
    if (state() == Connected)
    {
        m_controller.persistMetaData(CONFIGURATION);
    }
    std::for_each( m_windows.begin(), m_windows.end(),
                   std::mem_fun( &CharmWindow::configurationChanged ) );
}

ModelConnector& Application::model()
{
    return m_model;
}

DateChangeWatcher* Application::dateChangeWatcher() const
{
    return m_dateChangeWatcher;
}

IdleDetector* Application::idleDetector()
{
    return m_idleDetector;
}

void Application::setHttpActionsVisible( bool visible )
{
    m_actionSyncTasks.setVisible( visible );
}

void Application::slotMaybeIdle()
{
    if ( idleDetector() == 0 ) return; // should not happen

    if ( DATAMODEL->activeEventCount() > 0 ) {
        if ( idleDetector()->idlePeriods().count() == 1 ) {
            m_timeTracker.maybeIdle();
        } // otherwise, the dialog will be showing already
    }
    // there are four parameters to the idle property:
    // - the initial start time of the currently active event(s)
    // - the time the machine went idle
    // - the time it resumed from idling
    // - the current time
    // all this information is available in the data model, plus the
    // argument to this call
    // things that make it complicated:
    // - there may be multiple active events
    // - there may be multiple idle periods before the user deals with
    // it
}

CharmWindow& Application::mainView()
{
    return m_timeTracker;
}

TrayIcon& Application::trayIcon()
{
    return m_trayIcon;
}

void Application::slotCharmWindowVisibilityChanged( bool visible )
{
    if( !visible )
        m_closedWindow = dynamic_cast< CharmWindow* >( sender() );
}

void Application::saveState( QSessionManager & manager )
{
    //qDebug() << "saveState(QSessionManager)";
    if (m_state == Connected) {
        //QSettings settings;
        //const QString prefix = manager.sessionId() + '/';
        //qDebug() << "saveState" << prefix << "tasksWindow:" << m_tasksWindow.geometry();
        // Visibility is done already. TODO: desktop number?
        //settings.setValue(prefix + "tasks_window_shown", m_tasksWindow.isVisible());
        //settings.setValue(prefix + "event_window_shown", m_eventWindow.isVisible());
        //settings.setValue(prefix + "timetracker_window_shown", m_timeTracker.isVisible());
        //settings.setValue(prefix + "tasks_window_geometry", m_tasksWindow.geometry());

        m_tasksWindow.saveGuiState();
        m_eventWindow.saveGuiState();
        m_timeTracker.saveGuiState();
    } else {
        //qDebug() << "ignored";
    }
}

void Application::commitData( QSessionManager & manager )
{
    // Do nothing here. The default implementation closes all windows,
    // (just to see if the closeEvent is accepted), and this messes up
    // our saving of the "visible" state later on in saveData.
}

#include "Application.moc"
