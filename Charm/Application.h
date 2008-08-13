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

#include <QApplication>
#include <QMenu>
#include <QAction>

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

class Application : public QObject
{
    Q_OBJECT

public:
    explicit Application( int argc,  char** argv );
    ~Application();

    static Application& instance();

    int exec();

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

    State state() const;

public slots:
    void setState( State state );
    void slotQuitApplication();
    void slotControllerReadyToQuit();
    void slotSaveConfiguration();
    void slotGoToConnectedState();

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
    QApplication m_app;
    ModelConnector m_model;
    Controller m_controller;
    MainWindow m_mainWindow;
    TimeTrackingView m_timeTracker;

    // All statics are created as members of Application. This is
    // supposed to help on Windows, where constructors for statics
    // do not seem to called correctly.
    TimeSpans m_timeSpans;
    static Application* m_instance;
};

#endif