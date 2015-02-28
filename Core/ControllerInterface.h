/*
  ControllerInterface.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2007-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>

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

#ifndef CONTROLLERINTERFACE_H
#define CONTROLLERINTERFACE_H

#include <QDomDocument>

#include "Task.h"
#include "Event.h"
#include "State.h"

class CharmCommand;
class Configuration;
class StorageInterface;

class ControllerInterface
{
public:
    virtual ~ControllerInterface() {} // keep compiler happy

    // application:
    // react on application state changes
    virtual void stateChanged( State previous, State next ) = 0;
    // persist meta data portions of Configuration
    virtual void persistMetaData( Configuration& ) = 0;
    // load meta data and store appropriate portions in configuration
    virtual void provideMetaData( Configuration& ) = 0;

    /** Add an event.
        Return a valid event if successful. */
    virtual Event makeEvent( const Task& ) = 0;
    /** Add an event, copying data from another event. */
    virtual Event cloneEvent( const Event& ) = 0;
    /** Modify an event. */
    virtual bool modifyEvent( const Event& ) = 0;
    /** Delete an event. */
    virtual bool deleteEvent( const Event& ) = 0;
    /** Add a task, and send the result to the view as a signal. */
    virtual bool addTask( const Task& task ) = 0;
    /** Modify the task, the user has changed it in the view. */
    virtual bool modifyTask( const Task& ) = 0;
    /** Delete the task. Send a signal to the view confirming it. */
    virtual bool deleteTask( const Task& ) = 0;
    /** Set all tasks. Updates the view, after. */
    virtual bool setAllTasks( const TaskList& tasks ) = 0;
    /** Receive a command from the view. */
    virtual void executeCommand( CharmCommand* ) = 0;
    /** Receive an undo command from the view. */
    virtual void rollbackCommand( CharmCommand* ) = 0;
    /** Export the database contents into a XML document. */
    virtual QDomDocument exportDatabasetoXml() const = 0 ;
    /** Import the content of the Xml document into the currently open database.
     *  This will modify the database.
     *  @return An empty string on no error, an human-readable error message otherwise.
     */
    virtual QString importDatabaseFromXml( const QDomDocument& ) = 0;


    // supposed to be implemented as signals:
    /** Added an event. */
    virtual void eventAdded( const Event& event ) = 0;
    /** Modified an event. */
    virtual void eventModified( const Event& event ) = 0;
    /** Deleted an event. */
    virtual void eventDeleted( const Event& event ) = 0;
    /** This sends out the current task list. */
    virtual void definedTasks( const TaskList& ) = 0;
    /** Add a task. */
    virtual void taskAdded( const Task& ) = 0;
    /** Update a task in the view. */
    virtual void taskUpdated( const Task& ) = 0;
    /** Delete a task from the view completely. */
    virtual void taskDeleted( const Task& ) = 0;
    /** A command has been completed from the controller's point of view. */
    virtual void commandCompleted( CharmCommand* ) = 0;

    /** This tells the application that the controller is ready to quit.
        When the user quits the application, the application will tell
        the controller to end and commit all active events.
        The controller will emit readyToQuit() when all data is
        stored.
        The controller will leave Disconnecting state when it receives
        the signal. */
    virtual void readyToQuit() = 0;

    /** The currently used backend. */
    virtual StorageInterface* storage() = 0;

    /** Create the backend. */
    virtual bool initializeBackEnd( const QString& name ) = 0;
    /** Connect to the backend (make it available). */
    virtual bool connectToBackend() = 0;
    /** Disconnect from the backend (shut it down). */
    virtual bool disconnectFromBackend() = 0;
};

#endif
