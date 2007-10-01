#ifndef CONTROLLERINTERFACE_H
#define CONTROLLERINTERFACE_H

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
        Return a valid event if successfull. */
    virtual Event makeEvent( const Task& ) = 0;
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
    /** Receive a command from the view. */
    virtual void executeCommand( CharmCommand* ) = 0;

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
