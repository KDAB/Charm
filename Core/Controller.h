/*
  Controller.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2007-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>
  Author: Olivier JG <olivier.de.gaalon@kdab.com>
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

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>

#include "Event.h"
#include "Task.h"
#include "State.h"

class CharmCommand;
class Configuration;
class StorageInterface;

class Controller : public QObject
{
    Q_OBJECT

public:
    explicit Controller(QObject *parent = nullptr);
    ~Controller();

    // application:
    // react on application state changes
    void stateChanged(State previous, State next);

    // persist meta data portions of Configuration
    void persistMetaData(Configuration &);

    // load meta data and store appropriate portions in configuration
    void provideMetaData(Configuration &);

    /** Create the backend. */
    bool initializeBackEnd(const QString &name);

    /** Connect to the backend (make it available). */
    bool connectToBackend();

    /** Disconnect from the backend (shut it down). */
    bool disconnectFromBackend();

    /** The currently used backend. */
    StorageInterface *storage();

    // FIXME add the add/modify/delete functions will not be slots anymore

    /** Add an event.
        Return a valid event if successful. */
    Event makeEvent(const Task &);

    /** Add an event, copying data from another event. */
    Event cloneEvent(const Event &);

    /** Modify an event. */
    bool modifyEvent(const Event &);

    /** Delete an event. */
    bool deleteEvent(const Event &);

    /** Add a task, and send the result to the view as a signal. */
    bool addTask(const Task &parent);

    /** Modify the task, the user has changed it in the view. */
    bool modifyTask(const Task &);

    /** Delete the task. Send a signal to the view confirming it. */
    bool deleteTask(const Task &);

    /** Set all tasks. Updates the view, after. */
    bool setAllTasks(const TaskList &);

    /** Export the database contents into a XML document. */
    QDomDocument exportDatabasetoXml() const;

    /** Import the content of the Xml document into the currently open database.
     *  This will modify the database.
     *  @return An empty string on no error, an human-readable error message otherwise.
     */
    QString importDatabaseFromXml(const QDomDocument &);

    void updateModelEventsAndTasks();

public Q_SLOTS:
    /** Receive a command from the view. */
    void executeCommand(CharmCommand *);

    /** Receive an undo command from the view. */
    void rollbackCommand(CharmCommand *);

Q_SIGNALS:
    /** Added an event. */
    void eventAdded(const Event &event);

    /** Modified an event. */
    void eventModified(const Event &event);

    /** Deleted an event. */
    void eventDeleted(const Event &event);

    void allEvents(const EventList &);

    /** This sends out the current task list. */
    void definedTasks(const TaskList &);

    /** Add a task. */
    void taskAdded(const Task &);

    /** Update a task in the view. */
    void taskUpdated(const Task &);

    /** Delete a task from the view completely. */
    void taskDeleted(const Task &);

    /** This tells the application that the controller is ready to quit.
        When the user quits the application, the application will tell
        the controller to end and commit all active events.
        The controller will emit readyToQuit() when all data is
        stored.
        The controller will leave Disconnecting state when it receives
        the signal. */
    void readyToQuit();

    void currentBackendStatus(const QString &text);

    /** A command has been completed from the controller's point of view. */
    void commandCompleted(CharmCommand *);

private:
    void updateSubscriptionForTask(const Task &);

    template<class T> void loadConfigValue(const QString &key, T &configValue) const;
    StorageInterface *m_storage = nullptr;
};

#endif
