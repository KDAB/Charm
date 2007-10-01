#ifndef STORAGEINTERFACE_H
#define STORAGEINTERFACE_H

#include <QString>

#include "Task.h"
#include "User.h"
#include "State.h"
#include "Event.h"
#include "Installation.h"

class Event;
class Configuration;

class StorageInterface
{
public:
    virtual ~StorageInterface() {}

    // a readable description for the user
    virtual QString description() const = 0;

    // application:
    virtual void stateChanged( State previous ) = 0;

    // backend availability
    virtual bool connect( Configuration& ) = 0;
    virtual bool disconnect() = 0;

    // installation id table:
    // get the id of this installation
    virtual int installationId() const = 0;
    // create an installation id
    virtual Installation getInstallation( int installationId ) = 0;
    virtual Installation createInstallation( const QString& name ) = 0;
    virtual bool modifyInstallation( const Installation& ) = 0;
    virtual bool deleteInstallation( const Installation& ) = 0;

    // user database functions:
    virtual User getUser ( int userid ) = 0;
    virtual User makeUser( const QString& name ) = 0;
    virtual bool modifyUser ( const User& user ) = 0;
    virtual bool deleteUser ( const User& user ) = 0;

    // task database functions:
    virtual TaskList getAllTasks() = 0;
    virtual bool addTask( const Task& task ) = 0;
    virtual Task getTask( int taskId ) = 0;
    virtual bool modifyTask( const Task& task ) = 0;
    virtual bool deleteTask( const Task& task ) = 0;

    // event database functions:
    virtual EventList getAllEvents() = 0;
    // all events are created by the storage interface
    virtual Event makeEvent() = 0;
    virtual Event getEvent( int eventId )= 0;
    virtual bool modifyEvent( const Event& event ) = 0;
    virtual bool deleteEvent( const Event& event ) = 0;

    // subscription management functions
    // (subscriptions cannot be modified, they are just boolean flags)
    // (subscription status is retrieved with the tasks)
    virtual bool addSubscription( User, Task ) = 0;
    virtual bool deleteSubscription( User, Task ) = 0;

    // database metadata management functions
    virtual bool setMetaData( const QString& key, const QString& value ) = 0;
    virtual QString getMetaData( const QString& key ) = 0;

protected:
    // Put the basic database structure into the database.
    // This includes creating the tables et cetera.
    // Different backends will have to reimplement this function to
    // get special requirements in.
    // return true if successfull, false otherwise
    virtual bool createDatabase( Configuration& ) = 0;
};

#endif
