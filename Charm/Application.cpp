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

Application::Application(int argc, char** argv) :
	QObject(), m_state(Constructed), m_app(argc, argv), m_controller(),
			m_mainWindow()
{
	// QApplication setup
	m_app.setQuitOnLastWindowClosed(false);
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

	// exit process (app will only exit once controller says it is ready)
	connect(&m_controller, SIGNAL(readyToQuit()), SLOT(
			slotControllerReadyToQuit()));

	connectControllerAndModel(&m_controller, m_model.charmDataModel());
	connectControllerAndView(&m_controller, &m_mainWindow);

	// my own signals:
	connect(this, SIGNAL(goToState(State)), SLOT(setState(State)),
			Qt::QueuedConnection);

	// Ladies and gentlemen, please raise upon your seats -
	// the show is about to begin:
	emit goToState(StartingUp);
}

Application::~Application()
{
}

int Application::exec()
{
	return m_app.exec();
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
	// show view  (view is never invisible)
	m_mainWindow.show();
        // FIXME restore from previous setting:
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
		m_app.quit();
	}
}

void Application::leaveStartingUpState()
{
}

void Application::enterConnectingState()
{
    try {
	if (!m_controller.initializeBackEnd(CHARM_SQLITE_BACKEND_DESCRIPTOR))
            m_app.quit();
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
	QTimer::singleShot(1200, &m_app, SLOT(quit()));
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
                // m_app.quit();
                return false;
            }
	}

	return true;
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

#include "Application.moc"
