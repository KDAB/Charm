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
#include "ConfigurationDialog.h"

Application* Application::m_instance = 0;

Application::Application( int argc,  char** argv )
    : QObject ()
    , m_state( Constructed )
    , m_app( argc, argv )
    , m_controller()
    , m_view()
    , m_closing( false )
{
    // QApplication setup
    m_app.setQuitOnLastWindowClosed( false );
    // application metadata setup
    // note that this modifies the behaviour of QSettings:
    QCoreApplication::setOrganizationName( "KDAB" );
    QCoreApplication::setOrganizationDomain( "kdab.net" );
    QCoreApplication::setApplicationName( "Charm" );

    Q_INIT_RESOURCE( CharmResources );
    Q_ASSERT_X( m_instance == 0, "Application ctor",
                "Application is a singleton and cannot be created more than once" );
    m_instance = this;
    qRegisterMetaType<State>( "State" );
    // qRegisterMetaType<TaskId>( "TaskId" ); // unnecessary, only used in QVariant
    // qRegisterMetaType<TaskIdList>( "TaskIdList" );

    // window title updates
    connect( &m_controller, SIGNAL( currentBackendStatus( const QString& ) ),
             SLOT( slotCurrentBackendStatusChanged( const QString& ) ) );

    // save the configuration (configuration is managed by the application)
    connect( &m_view, SIGNAL( saveConfiguration() ),
             SLOT( slotSaveConfiguration() ) );
    // the exit process (close goes to systray, app->quit exits)
    connect( &m_view, SIGNAL( quit() ),
             SLOT( slotQuitApplication() ) );

    // exit process (app will only exit once controller says it is ready)
    connect( &m_controller, SIGNAL( readyToQuit() ),
             SLOT( slotControllerReadyToQuit() ) );

    connectControllerAndModel( &m_controller, m_model.charmDataModel() );
    connectControllerAndView( &m_controller, &m_view );

    // my own signals:
    connect( this, SIGNAL( goToState( State ) ), SLOT( setState(State ) ),
             Qt::QueuedConnection);

    // Ladies and gentlemen, please raise upon your seats -
    // the show is about to begin:
    emit goToState( StartingUp );
}

Application::~Application()
{
}

int Application::exec()
{
    return m_app.exec();
}

void Application::setState( State state )
{
    if ( m_state == state ) return;
#ifndef NDEBUG
    qDebug() << "Application::setState: going from" << StateNames[m_state]
             << "to" << StateNames[state];
#endif
    State previous = m_state;

    switch( m_state ) {
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
        Q_ASSERT_X( false, "Application::setState",
                    "Unknown previous application state" );
    };

    m_state = state;
    m_model.charmDataModel()->stateChanged( previous, state );
    m_controller.stateChanged( previous, state );
    m_view.stateChanged( previous );

    switch( m_state ) {
    case StartingUp:
        enterStartingUpState();
        break;
    case Connecting:
        enterConnectingState();
        break;
    case Connected:
        enterConnectedState();
        break;
    case Disconnecting:
        enterDisconnectingState();
        break;
    case ShuttingDown:
        enterShuttingDownState();
        break;
    default:
        Q_ASSERT_X( false, "Application::setState",
                    "Unknown new application state" );
    };
}

State Application::state() const
{
    return m_state;
}

Application& Application::instance()
{
    Q_ASSERT_X( m_instance, "Application::instance",
                "Singleton not constructed yet" );
    return * m_instance;
}

void Application::enterStartingUpState()
{
    // show view  (view is never invisible)
    m_view.show();
    // load configuration
    // ...
    // verify configuration
    // ...
    // if configuration is incomplete or buggy configure
    // FIXME ^^^
    // then go to connecting state
    if ( configure() )
    {   // if all ok, go to connecting state
        emit goToState( Connecting );
    } else {
        // user has cancelled configure, exit the application
        m_app.quit();
    }
}

void Application::leaveStartingUpState()
{
}

void Application::enterConnectingState()
{
    if ( !m_controller.initializeBackEnd( CHARM_SQLITE_BACKEND_DESCRIPTOR ) )
        m_app.quit();
    // tell storage to connect to database
    CONFIGURATION.failure = false;
    if ( m_controller.connectToBackend() )
    {
        slotSaveConfiguration();
        emit goToState( Connected );
    } else {
        // go back to StartingUp state and reconfigure
        emit goToState( StartingUp );
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
    m_view.setEnabled( false );
    // quit
    m_app.quit();
}

void Application::leaveShuttingDownState()
{
}

bool Application::configure()
{
    if ( CONFIGURATION.failure == true ) {
        qDebug() << "Application::configure: an error was found within the configuration.";
        if ( ! CONFIGURATION.failureMessage.isEmpty() ) {
            QMessageBox::information( &m_view, tr( "Configuration Problem" ),
                                      CONFIGURATION.failureMessage,
                                      tr( "Ok" ) );
            CONFIGURATION.failureMessage.clear();
        }
    }

    // load configuration:
    QSettings settings;
    settings.beginGroup( CONFIGURATION.configurationName );

    bool configurationComplete = CONFIGURATION.readFrom( settings );

    if ( !configurationComplete || CONFIGURATION.failure ) {
        qDebug() << "Application::configure: no complete configuration found for configuration name"
                 << CONFIGURATION.configurationName;
// FIXME maybe move to Configuration::loadDefaults
#ifdef NDEBUG
        CONFIGURATION.localStorageDatabase = QDir::homePath() + QDir::separator() + ".Charm/Charm.db";
#else
        CONFIGURATION.localStorageDatabase = QDir::homePath() + QDir::separator() + ".Charm/Charm_debug.db";
#endif
        ConfigurationDialog dialog( CONFIGURATION, &m_view );
        if ( dialog.exec() ) {
            CONFIGURATION = dialog.configuration();
            CONFIGURATION.writeTo( settings );
        } else {
            qDebug() << "Application::configure: user cancelled configuration. Exiting.";
            m_app.quit();
            return false;
        }
    }

    return true;
}

void Application::slotQuitApplication()
{
    m_closing = true;
    emit goToState( Disconnecting );
}

void Application::slotControllerReadyToQuit()
{
    emit goToState( ShuttingDown );
}

void Application::slotSaveConfiguration()
{
    QSettings settings;
    settings.beginGroup( CONFIGURATION.configurationName );
    CONFIGURATION.writeTo( settings );
    if ( state() == Connected ) {
        m_controller.persistMetaData( CONFIGURATION );
    }
}

void Application::slotCurrentBackendStatusChanged( const QString& text )
{
    QString title;
    QTextStream stream( &title );
    stream << "Charm ("
           << CONFIGURATION.user.name()
           << " - " << text << ")";

    m_view.setWindowTitle( title );
    // FIXME make work with systrayicon in view
    // m_trayIcon.setToolTip( title );
}

ModelConnector& Application::model()
{
    return m_model;
}

View& Application::view()
{
    return m_view;
}

TimeSpans& Application::timeSpans()
{
    return m_timeSpans;
}

#include "Application.moc"
