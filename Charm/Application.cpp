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
#include <QSettings>
#include <QMetaType>
#include <QMessageBox>

#include <Core/CharmConstants.h>
#include <Core/SqLiteStorage.h>

#include "ViewHelpers.h"
#include "Data.h"
#include "Application.h"
#include "SpecialKeysEventFilter.h"
#include "ConfigurationDialog.h"
#include "Idle/IdleDetector.h"

Application* Application::m_instance = 0;

#if defined Q_WS_MAC
extern void qt_mac_set_dock_menu(QMenu *);
#endif

Application::Application(int& argc, char** argv)
    : QApplication( argc, argv )
    , m_state(Constructed)
    , m_actionShowHideView( this )
    , m_actionShowHideTimeTracker( this )
    , m_actionStopAllTasks( this )
    , m_idleDetector( 0 )
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

    // save the configuration (configuration is managed by the application)
    connect(&m_mainWindow, SIGNAL(saveConfiguration()), SLOT(
                slotSaveConfiguration()));
    // the exit process (close goes to systray, app->quit exits)
    connect(&m_mainWindow, SIGNAL(quit()), SLOT(slotQuitApplication()));
    // window visibilities:
    connect( &m_mainWindow, SIGNAL( visibilityChanged( bool ) ),
             SLOT( slotMainWindowVisibilityChanged( bool ) ) );
    connect( &m_timeTracker, SIGNAL( visibilityChanged( bool ) ),
             SLOT( slotTimeTrackerVisibilityChanged( bool ) ) );
    // window title updates
    connect( &m_controller, SIGNAL( currentBackendStatus( const QString& ) ),
             SLOT( slotCurrentBackendStatusChanged( const QString& ) ) );

    // exit process (app will only exit once controller says it is ready)
    connect(&m_controller, SIGNAL(readyToQuit()), SLOT(
                slotControllerReadyToQuit()));

    connectControllerAndModel(&m_controller, m_model.charmDataModel());
    connectControllerAndView(&m_controller, &m_mainWindow);

    // my own signals:
    connect(this, SIGNAL(goToState(State)), SLOT(setState(State)),
            Qt::QueuedConnection);

    // system tray icon:
    m_actionStopAllTasks.setText( tr( "Stop &All Active Tasks" ) );
    m_actionStopAllTasks.setShortcut( Qt::Key_Escape );
    m_actionStopAllTasks.setShortcutContext( Qt::ApplicationShortcut );
    m_mainWindow.addAction(&m_actionStopAllTasks); // for the shortcut to work
    connect( &m_actionStopAllTasks, SIGNAL( triggered() ),
             SLOT( slotStopAllTasks() ) );
    connect( &m_actionShowHideView, SIGNAL( triggered() ),
             &m_mainWindow, SLOT( slotShowHideView() ) );
    connect( &m_actionShowHideTimeTracker, SIGNAL( triggered() ),
             &m_timeTracker, SLOT( slotShowHide() ) );
    connect( &m_trayIcon, SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ),
             SLOT( slotTrayIconActivated( QSystemTrayIcon::ActivationReason ) ) );
    m_systrayContextMenu.addAction( &m_actionShowHideView );
#ifdef TIMETRACKER_TEMPORARILY_DISABLED
    m_systrayContextMenu.addAction( &m_actionShowHideTimeTracker );
#endif
    m_systrayContextMenu.addAction( &m_actionStopAllTasks );
    m_systrayContextMenu.addSeparator();
    m_systrayContextMenu.addAction( m_mainWindow.actionQuit() );
    m_trayIcon.setContextMenu( &m_systrayContextMenu );
    m_trayIcon.setIcon( Data::charmIcon() );
    m_trayIcon.show();

#if defined Q_WS_MAC
    m_dockMenu.addAction( &m_actionShowHideView );
#ifdef TIMETRACKER_TEMPORARILY_DISABLED
    m_dockMenu.addAction( &m_actionShowHideTimeTracker );
#endif
    m_dockMenu.addAction( &m_actionStopAllTasks );
    qt_mac_set_dock_menu( &m_dockMenu);
#endif

    // FIXME time tracker is disabled for now
    m_actionShowHideTimeTracker.setEnabled( false );
    m_timeTracker.hide();
    SpecialKeysEventFilter* filter = new SpecialKeysEventFilter( this );
    installEventFilter( filter );
    connect( filter, SIGNAL( toggleWindow1Visibility() ),
             &m_mainWindow, SLOT( slotShowHideView() ) );
#ifdef TIMETRACKER_TEMPORARILY_DISABLED
    connect( filter, SIGNAL( toggleWindow2Visibility() ),
             &m_timeTracker, SLOT( slotShowHide() ) );
#endif
    // ^^^

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

void Application::setState(State state)
{
    if (m_state == state)
        return;
#ifndef NDEBUG
    qDebug() << "Application::setState: going from" << StateNames[m_state]
             << "to" << StateNames[state];
#endif
    State previous = m_state;

    switch (m_state)
    {
    case Constructed:
        break; // ignore, but this state is never re-entered
    case StartingUp:
        leaveStartingUpState();
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

    switch (m_state)
    {
    case StartingUp:
        m_model.charmDataModel()->stateChanged(previous, state);
        m_controller.stateChanged(previous, state);
        m_mainWindow.stateChanged(previous);
        m_timeTracker.stateChanged( previous );
        enterStartingUpState();
        break;
    case Connecting:
        m_model.charmDataModel()->stateChanged(previous, state);
        m_controller.stateChanged(previous, state);
        m_mainWindow.stateChanged(previous);
        m_timeTracker.stateChanged( previous );
        enterConnectingState();
        break;
    case Connected:
        m_model.charmDataModel()->stateChanged(previous, state);
        m_controller.stateChanged(previous, state);
        m_mainWindow.stateChanged(previous);
        m_timeTracker.stateChanged( previous );
        enterConnectedState();
        break;
    case Disconnecting:
        m_timeTracker.stateChanged( previous );
        m_mainWindow.stateChanged(previous);
        m_model.charmDataModel()->stateChanged(previous, state);
        m_controller.stateChanged(previous, state);
        enterDisconnectingState();
        break;
    case ShuttingDown:
        m_timeTracker.stateChanged( previous );
        m_mainWindow.stateChanged(previous);
        m_model.charmDataModel()->stateChanged(previous, state);
        m_controller.stateChanged(previous, state);
        enterShuttingDownState();
        break;
    default:
        Q_ASSERT_X(false, "Application::setState",
                   "Unknown new application state");
    };
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
    // HACK (Qt Mac menu merge bug)  necessary to merge main window menu?
    m_mainWindow.show();
    m_timeTracker.show();
    // load configuration
    // ...
    // verify configuration
    // ...
    // if configuration is incomplete or buggy configure
    // FIXME ^^^
    // then go to connecting state
    if (configure())
    { // if all ok, go to connecting state
        emit goToState(Connecting);
    }
    else
    {
        // user has cancelled configure, exit the application
        quit();
    }
}

void Application::leaveStartingUpState()
{
}

void Application::enterConnectingState()
{
    try {
	if (!m_controller.initializeBackEnd(CHARM_SQLITE_BACKEND_DESCRIPTOR))
            quit();
    } catch ( CharmException& e ) {
        QMessageBox::critical(&m_mainWindow, QObject::tr("Database Backend Error"),
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
            QTimer::singleShot(1200, this, SLOT(slotGoToConnectedState()));
        }
        else
        {
            // go back to StartingUp state and reconfigure
            emit goToState(StartingUp);
        }
    } catch (UnsupportedDatabaseVersionException& e) {
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
        QMessageBox::critical(&m_mainWindow, QObject::tr("Charm Database Error"),
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
    slotSaveConfiguration();
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
    // prevent all modules from accepting any user commands
    m_mainWindow.setEnabled(false);
    m_timeTracker.setEnabled( false );
    QTimer::singleShot(1200, this, SLOT(quit()));
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
            QMessageBox::information(&m_mainWindow,
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
        ConfigurationDialog dialog(CONFIGURATION, &m_mainWindow);
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
        m_mainWindow.slotShowHideView();
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

void Application::slotMainWindowVisibilityChanged( bool visible )
{
    if ( visible ) {
        m_actionShowHideView.setText( tr( "Hide Charm Window" ) );
    } else {
        m_actionShowHideView.setText( tr( "Show Charm Window" ) );
    }
}

void Application::slotTimeTrackerVisibilityChanged( bool visible )
{
    if ( visible ) {
        m_actionShowHideTimeTracker.setText( tr( "Hide Time Tracker Window" ) );
    } else {
        m_actionShowHideTimeTracker.setText( tr( "Show Time Tracker Window" ) );
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
    m_mainWindow.setWindowTitle( title );
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
}

ModelConnector& Application::model()
{
    return m_model;
}

MainWindow& Application::view()
{
    return m_mainWindow;
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
            m_mainWindow.maybeIdle();
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

#include "Application.moc"
