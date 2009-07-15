#ifndef CHARM_APPLICATION_H
#define CHARM_APPLICATION_H

/*
 *  Application.h
 *  Charm
 *
 *  Created by Mirko Boehm on 9/15/06.
 *  Copyright 2006 KDAB. All rights reserved.
 *
 */

#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#ifdef Q_WS_MAC
#include "mac_application.h"
#else
#include <QApplication>
#endif

// this is an application, not a library:
// no pimpling, and data members instead of forward declarations
#include "Core/User.h"
#include "Core/State.h"
#include "Core/TimeSpans.h"
#include "Core/Controller.h"
#include "Core/Configuration.h"
#include "Core/StorageInterface.h"

#include "CharmWindow.h"
#include "TasksWindow.h"
#include "EventWindow.h"
#include "TimeTrackingView.h"
#include "ModelConnector.h"

// FIXME read configuration name from command line
class IdleDetector;

#ifdef Q_WS_MAC
typedef MacApplication ApplicationBase;
#else
typedef QApplication ApplicationBase;
#endif

class Application : public ApplicationBase
{
    Q_OBJECT

public:
    explicit Application( int& argc,  char** argv );
    ~Application();

    static Application& instance();

    // FIXME broken by design?
    /** Configure the application.
        Returns true if configuring failed.
        The application can only leave StartingUp state once a valid
        configuration is available. */
    bool configure();

    /** Access to the model. */
    ModelConnector& model();

    /** Access to the time spans object. */
    TimeSpans& timeSpans();

    IdleDetector* idleDetector();

    State state() const;

    QMenu& windowMenu();
    QMenu& fileMenu();
    /** The main view is the window responsible for managing state during command execution.
     * It is an internal concept, not a notion for the end user. */
    CharmWindow& mainView();

    /*! \reimp */ void saveState( QSessionManager & manager );
    /*! \reimp */ void commitData( QSessionManager & manager );

public slots:
    void setState( State state );
    void slotQuitApplication();
    void slotControllerReadyToQuit();
    void slotSaveConfiguration();
    void slotGoToConnectedState();

private slots:
    void slotTrayIconActivated( QSystemTrayIcon::ActivationReason );
//     void slotMainWindowVisibilityChanged( bool );
//     void slotTimeTrackerVisibilityChanged( bool );
    void slotCurrentBackendStatusChanged( const QString& text );
    void slotStopAllTasks();
    void slotMaybeIdle();

    void slotOpenLastClosedWindow();
    void slotCharmWindowVisibilityChanged( bool visibility );

signals:
    void goToState( State state );

private:
    void enterStartingUpState();
    void leaveStartingUpState();
    void enterConfiguringState();
    void leaveConfiguringState();
    void enterConnectingState();
    void leaveConnectingState();
    void enterConnectedState();
    void leaveConnectedState();
    void enterDisconnectingState();
    void leaveDisconnectingState();
    void enterShuttingDownState();
    void leaveShuttingDownState();

    State m_state;
    ModelConnector m_model;
    Controller m_controller;
    // MainWindow m_mainWindow;
    TasksWindow m_tasksWindow;
    EventWindow m_eventWindow;
    TimeTrackingView m_timeTracker;
    QSystemTrayIcon m_trayIcon;
    QMenu m_systrayContextMenu;
    QMenu m_dockMenu;
    QMenu m_windowMenu;
    QMenu m_appMenu;
    // QAction m_actionShowHideView;
    // QAction m_actionShowHideTimeTracker;
    QAction m_actionStopAllTasks;
    QAction m_actionQuit;
    QAction m_actionAboutDialog;
    QAction m_actionPreferences;
    QAction m_actionExportToXml;
    QAction m_actionImportFromXml;
    QAction m_actionImportTasks;
    QAction m_actionReporting;

    IdleDetector* m_idleDetector;
    CharmWindow* m_closedWindow;
    const QList<CharmWindow*> m_windows;

    // All statics are created as members of Application. This is
    // supposed to help on Windows, where constructors for statics
    // do not seem to called correctly.
    TimeSpans m_timeSpans;
    static Application* m_instance;
};

#endif
