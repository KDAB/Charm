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
#include <QLocalServer>

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
#include "TimeTrackingView/TimeTrackingWindow.h"
#include "ModelConnector.h"
#include "TrayIcon.h"

// FIXME read configuration name from command line

class IdleDetector;

class Application : public QApplication
{
    Q_OBJECT

public:
    explicit Application( int& argc,  char** argv );
    ~Application();

    static Application& instance();
    static bool hasInstance();

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

    void createWindowMenu( QMenuBar *menuBar );
    void createFileMenu( QMenuBar *menuBar );
    void createHelpMenu( QMenuBar *menuBar );

    /** The main view is the window responsible for managing state during command execution.
     * It is an internal concept, not a notion for the end user. */
    CharmWindow& mainView();

    TrayIcon& trayIcon();

    /*! \reimp */ void saveState( QSessionManager & manager );
    /*! \reimp */ void commitData( QSessionManager & manager );

public slots:
    void setState( State state );
    void slotStopAllTasks();
    void slotQuitApplication();
    void slotControllerReadyToQuit();
    void slotSaveConfiguration();
    void slotGoToConnectedState();

    void toggleShowHide();

private slots:
//     void slotMainWindowVisibilityChanged( bool );
//     void slotTimeTrackerVisibilityChanged( bool );
    void slotCurrentBackendStatusChanged( const QString& text );
    void slotMaybeIdle();
    void slotCharmWindowVisibilityChanged( bool visibility );
    void slotHandleUniqueApplicationConnection();

signals:
    void goToState( State state );

protected:
    void openAWindow( bool raise = false );

    CharmWindow* m_closedWindow;
    QAction m_actionStopAllTasks;
    const QList<CharmWindow*> m_windows;
    TimeTrackingWindow m_timeTracker;
    QAction m_actionQuit;

private:
    QString titleString( const QString& text ) const;
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
    TrayIcon m_trayIcon;
    QMenu m_systrayContextMenu;
    QAction m_actionAboutDialog;
    QAction m_actionPreferences;
    QAction m_actionExportToXml;
    QAction m_actionImportFromXml;
    QAction m_actionImportTasks;
    QAction m_actionReporting;
    TasksWindow m_tasksWindow;
    EventWindow m_eventWindow;
    IdleDetector* m_idleDetector;
    bool m_timeTrackerHiddenFromSystrayToggle;
    bool m_tasksWindowHiddenFromSystrayToggle;
    bool m_eventWindowHiddenFromSystrayToggle;
    QLocalServer m_uniqueApplicationServer;

    // All statics are created as members of Application. This is
    // supposed to help on Windows, where constructors for statics
    // do not seem to called correctly.
    TimeSpans m_timeSpans;
    static Application* m_instance;
};

#endif
