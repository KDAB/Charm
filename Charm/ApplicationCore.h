/*
  ApplicationCore.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2014-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>
  Author: Frank Osterfeld <frank.osterfeld@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef APPLICATIONCORE_H
#define APPLICATIONCORE_H

#include <QMenu>
#include <QAction>
#include <QLocalServer>

// this is an application, not a library:
// no pimpling, and data members instead of forward declarations
#include "Core/User.h"
#include "Core/State.h"
#include "Core/TimeSpans.h"
#include "Core/Controller.h"
#include "Core/Configuration.h"
#include "Core/SqlStorage.h"

#include "Widgets/CharmWindow.h"
#include "Widgets/EventView.h"
#include "Widgets//TimeTrackingWindow.h"
#include "Widgets/TasksView.h"
#include "Widgets/TrayIcon.h"

#include "ModelConnector.h"

// FIXME read configuration name from command line

class CharmCommandInterface;
class IdleDetector;
class QSessionManager;
class QWinJumpList;

class ApplicationCore : public QObject
{
    Q_OBJECT

public:
    enum class ShowMode {
        Show,
        ShowAndRaise
    };
    explicit ApplicationCore(TaskId startupTask, QObject *parent = nullptr);
    ~ApplicationCore() override;

    static ApplicationCore &instance();
    static bool hasInstance();

    // FIXME broken by design?
    /** Configure the application.
        Returns true if configuring failed.
        The application can only leave StartingUp state once a valid
        configuration is available. */
    bool configure();

    /** Access to the model. */
    ModelConnector &model();

    /** Access to the time spans object. */
    DateChangeWatcher *dateChangeWatcher() const;

    IdleDetector *idleDetector();
    CharmCommandInterface *commandInterface() const;
    State state() const;

    void createWindowMenu(QMenuBar *menuBar);
    void createFileMenu(QMenuBar *menuBar);
    void createHelpMenu(QMenuBar *menuBar);

    /** The main view is the window responsible for managing state during command execution.
     * It is an internal concept, not a notion for the end user. */
    CharmWindow &mainView();

    TrayIcon &trayIcon();

    void updateTaskList();

public Q_SLOTS:
    void showMainWindow(ShowMode mode = ShowMode::Show);

    void setState(State state);
    void slotStopAllTasks();
    void slotQuitApplication();
    void slotControllerReadyToQuit();
    void slotSaveConfiguration();
    void slotGoToConnectedState();
    void setHttpActionsVisible(bool visible);
    void saveState(QSessionManager &manager);
    void commitData(QSessionManager &manager);

private Q_SLOTS:
    void slotCurrentBackendStatusChanged(const QString &text);
    void slotMaybeIdle();
    void slotHandleUniqueApplicationConnection();
    void slotStartTaskMenuAboutToShow();
    void slotShowNotification(const QString &title, const QString &message);
    void slotShowTasksEditor();
    void slotShowEventEditor();

Q_SIGNALS:
    void goToState(State state);

protected:
    QAction m_actionStopAllTasks;
    TimeTrackingWindow m_timeTracker;
    QAction m_actionQuit;

private:
    void showCritical(const QString &title, const QString &message);
    void showInformation(const QString &title, const QString &message);

    QString titleString(const QString &text) const;
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

    State m_state = Constructed;
    ModelConnector m_model;
    Controller m_controller;
    TrayIcon m_trayIcon;
    QMenu m_systrayContextMenu;
    QAction m_actionAboutDialog;
    QAction m_actionPreferences;
    QAction m_actionExportToXml;
    QAction m_actionImportFromXml;
    QAction m_actionSyncTasks;
    QAction m_actionImportTasks;
    QAction m_actionExportTasks;
    QAction m_actionCheckForUpdates;
    QAction m_actionEnterVacation;
    QAction m_actionActivityReport;
    QAction m_actionWeeklyTimesheetReport;
    QAction m_actionMonthlyTimesheetReport;
    QList<QAction *> m_taskActions;
    EventView m_eventView;
    TasksView m_tasksView;
    QVector<UIStateInterface *> m_uiElements;
    IdleDetector *m_idleDetector = nullptr;
    CharmCommandInterface *m_cmdInterface = nullptr;
    QLocalServer m_uniqueApplicationServer;
    TaskId m_startupTask;
#ifdef Q_OS_WIN
    QWinJumpList *m_windowsJumpList = nullptr;
#endif

    // All statics are created as members of Application. This is
    // supposed to help on Windows, where constructors for statics
    // do not seem to called correctly.
    DateChangeWatcher *m_dateChangeWatcher;
    static ApplicationCore *m_instance;
};

#endif
