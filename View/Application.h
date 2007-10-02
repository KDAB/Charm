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
#include "View.h"
#include "Core/User.h"
#include "Core/State.h"
#include "Model.h"
#include "Core/TimeSpans.h"
#include "Core/Controller.h"
#include "Core/Configuration.h"
#include "Core/StorageInterface.h"

// FIXME read configuration name from command line

class Application : public QObject
{
    Q_OBJECT

public:
    explicit Application( int argc,  char** argv );
    ~Application();

    static Application& instance();

    int exec();

    /** Configure the application.
        Returns true if configuring failed.
        The application can only leave StartingUp state once a valid
        configuration is available. */
    bool configure();

    /** Access to the model. */
    Model& model();

    /** Access to the view. */
    View& view();

    /** Access to the time spans object. */
    TimeSpans& timeSpans();

    State state() const;

public slots:
    void setState( State state );
    void slotQuitApplication();
    void slotControllerReadyToQuit();
    void slotSaveConfiguration();
    void slotCurrentBackendStatusChanged( const QString& text );

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
    Model m_model;
    Controller m_controller;
    View m_view;
    TimeSpans m_timeSpans;

    bool m_closing;
    static Application* m_instance;
};

#endif
