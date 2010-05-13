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
#include <QMessageBox>
#include <QSessionManager>
#include <QSystemTrayIcon>

#include <Core/CharmConstants.h>
#include <Core/SqLiteStorage.h>

#include "ViewHelpers.h"
#include "Data.h"
#include "Application.h"
#include "ConfigurationDialog.h"
#include "Idle/IdleDetector.h"
#include "Uniquifier.h"

#include <algorithm> //for_each()

Application* Application::m_instance = 0;

#if defined Q_WS_MAC
extern void qt_mac_set_dock_menu(QMenu *);
#endif

Application::Application(int& argc, char** argv)
    : ApplicationBase( argc, argv )
    , m_state(Constructed)
    , m_actionStopAllTasks( this )
    , m_actionQuit( this )
    , m_actionAboutDialog( this )
    , m_actionPreferences( this )
    , m_actionExportToXml( this )
    , m_actionImportFromXml( this )
    , m_actionImportTasks( this )
    , m_actionReporting( this )
    , m_idleDetector( 0 )
    , m_closedWindow( 0 )
    , m_windows( QList<CharmWindow*> () << &m_tasksWindow << &m_eventWindow << &m_timeTracker )
{
    // QApplication setup
    setQuitOnLastWindowClosed(false);
    // application metadata setup
    // note that this modifies the behaviour of QSettings:
    QCoreApplication::setOrganizationName("KDAB");
    QCoreApplication::setOrganizationDomain("kdab.net");
    QCoreApplication::setApplicationName("Charm");

    Q_INIT_RESOURCE(CharmResources);
    Q_ASSERT_X(m_instance == 0, "Application ctor",
               "Application is a singleton and cannot be created more than once");
    m_instance = this;
    qRegisterMetaType<State> ("State");
    qRegisterMetaType<Event> ("Event");

    // MIRKO_TEMP_REM needed?
    /*
    // window title updates
    connect( &m_controller, SIGNAL( currentBackendStatus( const QString& ) ),
    SLOT( slotCurrentBackendStatusChanged( const QString& ) ) );
    */
    // exit process (app will only exit once controller says it is ready)
    connect(&m_controller, SIGNAL(readyToQuit()), SLOT(
                slotControllerReadyToQuit()));

    connectControllerAndModel(&m_controller, m_model.charmDataModel());
    connectControllerAndView(&m_controller, &m_tasksWindow);
    Q_FOREACH( CharmWindow* window, m_windows ) {
        if ( window != &m_tasksWindow ) { // tasks view acts as the main relay
            connect( window, SIGNAL( emitCommand( CharmCommand* ) ),
                     &mainView(), SLOT( sendCommand( CharmCommand* ) ) );
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
    m_timeTracker.addAction(&m_actionStopAllTasks); // for the shortcut to work
    connect( &m_actionStopAllTasks, SIGNAL( triggered() ),
             SLOT( slotStopAllTasks() ) );
    connect( &m_trayIcon, SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ),
             SLOT( slotTrayIconActivated( QSystemTrayIcon::ActivationReason ) ) );
    m_systrayContextMenu.addAction( &m_actionStopAllTasks );
    m_systrayContextMenu.addSeparator();

    // MIRKO_TEMP_REM where to put quit action?
    // m_systrayContextMenu.addAction( m_mainWindow.actionQuit() );
    m_trayIcon.setContextMenu( &m_systrayContextMenu );
    m_trayIcon.setIcon( Data::charmTrayIcon() );
    m_trayIcon.show();

#if defined Q_WS_MAC
    m_dockMenu.addAction( &m_actionStopAllTasks );
    qt_mac_set_dock_menu( &m_dockMenu);
#endif

    Q_FOREACH( CharmWindow* window, m_windows ) {
        m_systrayContextMenu.addAction( window->showHideAction() );
#if defined Q_WS_MAC
        m_dockMenu.addAction( window->showHideAction() );
#endif
    }

    // set up actions:
    m_actionQuit.setShortcut( Qt::CTRL + Qt::Key_Q );
    m_actionQuit.setText( tr( "Quit" ) );
    m_actionQuit.setIcon( Data::quitCharmIcon() );
    connect( &m_actionQuit, SIGNAL( triggered( bool ) ),
             SLOT( slotQuitApplication() ) );

    m_actionAboutDialog.setText( tr( "About Charm" ) );
    connect( &m_actionAboutDialog, SIGNAL( triggered() ),
             &m_tasksWindow,  SLOT( slotAboutDialog() ) );

    m_actionPreferences.setText( tr( "Preferences" ) );
    m_actionPreferences.setIcon( Data::configureIcon() );
    connect( &m_actionPreferences, SIGNAL( triggered( bool ) ),
             &m_tasksWindow,  SLOT( slotEditPreferences( bool ) ) );

    m_actionImportFromXml.setText( tr( "Import from Previous Export..." ) );
    connect( &m_actionImportFromXml, SIGNAL( triggered() ),
             &m_tasksWindow,  SLOT( slotImportFromXml() ) );
    m_actionExportToXml.setText( tr( "Export..." ) );
    connect( &m_actionExportToXml, SIGNAL( triggered() ),
             &m_tasksWindow,  SLOT( slotExportToXml() ) );
    m_actionImportTasks.setText( tr( "Import Task Definitions..." ) );
    connect( &m_actionImportTasks, SIGNAL( triggered() ),
             &m_tasksWindow,  SLOT( slotImportTasks() ) );

    // set up Charm menu:
    m_appMenu.setTitle ( tr( "File" ) );
    m_appMenu.addAction( &m_actionPreferences );
    m_actionPreferences.setEnabled( true );
    m_appMenu.addAction( &m_actionAboutDialog );
    m_appMenu.addSeparator();
    m_appMenu.addAction( &m_actionExportToXml );
    m_appMenu.addAction( &m_actionImportFromXml );
    m_appMenu.addSeparator();
    m_appMenu.addAction( &m_actionImportTasks );
    m_appMenu.addSeparator();
    m_appMenu.addAction( &m_actionQuit );

    // create window menu:
    m_windowMenu.setTitle( tr( "Window" ) );
    Q_FOREACH( CharmWindow* window, m_windows ) {
        m_windowMenu.addAction( window->showHideAction() );
    }
    m_actionReporting.setText( tr( "Reports..." ) );
    m_actionReporting.setShortcut( Qt::CTRL + Qt::Key_R );
    connect( &m_actionReporting, SIGNAL( triggered() ),
             &m_tasksWindow, SLOT( slotReportDialog() ) );
    m_windowMenu.addSeparator();
    m_windowMenu.addAction( &m_actionReporting );

#ifdef Q_WS_MAC
    connect( QApplication::instance(), SIGNAL( dockIconClicked() ), this, SLOT( slotOpenLastClosedWindow() ) );
#endif

    // set up idle detection
    m_idleDetector = IdleDetector::createIdleDetector( this );
    if ( m_idleDetector == 0 ) {
        qDebug() << "Application ctor: idle detection is not available on this platform.";
    } else {
        qDebug() << "Application ctor: idle detection initialized.";
        connect( m_idleDetector, SIGNAL( maybeIdle() ), SLOT( slotMaybeIdle() ) );
    }

    // Ladies and gentlemen, please raise upon your seats -
    // the show is about to begin:
    emit goToState(StartingUp);
}

Application::~Application()
{
}

QMenu& Application::windowMenu()
{
    return m_windowMenu;
}

QMenu& Application::fileMenu()
{
    return m_appMenu;
}

void Application::setState(State state)
{
    if (m_state == state)
        return;
#ifndef NDEBUG
    qDebug() << "Application::setState: going from" << StateNames[m_state]
             << "to" << StateNames[state];
#endif
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
    } catch( CharmException& e ) {
        QMessageBox::critical( &m_tasksWindow, tr( "Critical Charm Problem" ),
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

void Application::enterStartingUpState()
{
#ifdef QT_MAC_USE_COCOA
    // Need to setup the Cocoa event handler after the default Apple ones
    // so can't be done in the MacCocoaApplication constructor.
    setupCocoaEventHandler();
#endif

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
    } catch ( CharmException& e ) {
        QMessageBox::critical( & m_tasksWindow, QObject::tr("Database Backend Error"),
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
    } catch (UnsupportedDatabaseVersionException&) {
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
        QMessageBox::critical( & m_tasksWindow, QObject::tr("Charm Database Error"),
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
bool Application::configure()
{
    if (CONFIGURATION.failure == true)
    {
        qDebug()
            << "Application::configure: an error was found within the configuration.";
        if (!CONFIGURATION.failureMessage.isEmpty())
        {
            QMessageBox::information( &m_tasksWindow,
                                     tr("Configuration Problem"), CONFIGURATION.failureMessage,
                                     tr("Ok"));
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
#ifdef NDEBUG
        CONFIGURATION.localStorageDatabase = QDir::homePath() + QDir::separator() + ".Charm/Charm.db";
#else
        CONFIGURATION.localStorageDatabase = QDir::homePath()
                                             + QDir::separator() + ".Charm/Charm_debug.db";
#endif
        ConfigurationDialog dialog(CONFIGURATION, &m_tasksWindow);
        if (dialog.exec())
        {
            CONFIGURATION = dialog.configuration();
            CONFIGURATION.writeTo(settings);
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

void Application::slotTrayIconActivated( QSystemTrayIcon::ActivationReason reason )
{
    switch( reason ) {
    case QSystemTrayIcon::Context:
        // show context menu
        // m_systrayContextMenu.show();
        break;
    case QSystemTrayIcon::DoubleClick:
        // MIRKO_TEMP_REM?
        /*
        m_mainWindow.slotShowHideView();
        */
        break;
    case QSystemTrayIcon::Trigger:
        // single click
        break;
    case QSystemTrayIcon::MiddleClick:
        // ...
        break;
    case QSystemTrayIcon::Unknown:
    default:
        break;
    }
}

void Application::slotCurrentBackendStatusChanged( const QString& text )
{   // FIXME why can't this be done on stateChanged()? and if not, is
    // maybe an app-wide metadataChanged() or configurationChanged()
    // missing? (the latter exists)
    QString dbInfo;
    const QString userName = CONFIGURATION.user.name();
    if (!userName.isEmpty())
        dbInfo = QString("%1 - %2").arg(userName, text);
    else
        dbInfo = text;

    const QString title = tr("Charm (%1)").arg(dbInfo);
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

TimeSpans& Application::timeSpans()
{
    return m_timeSpans;
}

IdleDetector* Application::idleDetector()
{
    return m_idleDetector;
}

void Application::slotMaybeIdle()
{
    if ( idleDetector() == 0 ) return; // should not happen

    if ( DATAMODEL->activeEventCount() > 0 ) {
        if ( idleDetector()->idlePeriods().count() == 1 ) {
            m_tasksWindow.maybeIdle();
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
    // FIXME is this any good?
    return m_tasksWindow;
}

void Application::slotOpenLastClosedWindow()
{
    if( m_closedWindow == 0 )
        return;
    m_closedWindow->show();
    m_closedWindow = 0;
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
