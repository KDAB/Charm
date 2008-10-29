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

#include <QMenu>
#include <QAction>
#include <QApplication>

// this is an application, not a library:
// no pimpling, and data members instead of forward declarations
#include "Core/User.h"
#include "Core/State.h"
#include "Core/TimeSpans.h"
#include "Core/Controller.h"
#include "Core/Configuration.h"
#include "Core/StorageInterface.h"

#include "MainWindow.h"
#include "TimeTrackingView.h"
#include "ModelConnector.h"

// FIXME read configuration name from command line
class IdleDetector;

class Application : public QApplication
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

    /** Access to the view. */
    MainWindow& view();

    /** Access to the time spans object. */
    TimeSpans& timeSpans();

    IdleDetector* idleDetector();

    State state() const;

public slots:
    void setState( State state );
    void slotQuitApplication();
    void slotControllerReadyToQuit();
    void slotSaveConfiguration();
    void slotGoToConnectedState();

private slots:
    void slotTrayIconActivated( QSystemTrayIcon::ActivationReason );
    void slotMainWindowVisibilityChanged( bool );
    void slotTimeTrackerVisibilityChanged( bool );
    void slotCurrentBackendStatusChanged( const QString& text );
    void slotStopAllTasks();
    void slotMaybeIdle();

signals:
    void goToState( State state );

private:
    void enterStartingUpState();
    void leaveStartingUpState();
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
    MainWindow m_mainWindow;
    TimeTrackingView m_timeTracker;
    QSystemTrayIcon m_trayIcon;
    QMenu m_systrayContextMenu;
    QMenu m_dockMenu;
    QAction m_actionShowHideView;
    QAction m_actionShowHideTimeTracker;
    QAction m_actionStopAllTasks;

    IdleDetector* m_idleDetector;

    // All statics are created as members of Application. This is
    // supposed to help on Windows, where constructors for statics
    // do not seem to called correctly.
    TimeSpans m_timeSpans;
    static Application* m_instance;
};

#endif
