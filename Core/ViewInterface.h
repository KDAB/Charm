#ifndef CHARM_VIEWINTERFACE_H
#define CHARM_VIEWINTERFACE_H

#include "Task.h"
#include "State.h"
#include "Event.h"

class CharmCommand;

class ViewInterface
{
public:
    virtual ~ViewInterface() {} // keep compiler happy

    // application:
    virtual void stateChanged( State previous ) = 0;
    virtual void saveConfiguration() = 0;

    // virtual void addTaskRequested( const Task& parent ) = 0;
    // virtual void deleteTaskRequested( const Task& task ) = 0;

    virtual void emitCommand( CharmCommand* ) = 0;
    virtual void sendCommand( CharmCommand* ) = 0;

    // restore the view
    virtual void restore() = 0;
    // quit the application
    virtual void quit() = 0;

    // these are supposed to be overloaded as slots:
    // virtual void setTaskList( const TaskList& ) = 0;
    // virtual void setCurrentEvents( const EventList& ) = 0;
    // virtual void receiveAddTask( const Task& ) = 0;
    // virtual void receiveModifyTask( const Task& ) = 0;
    // virtual void receiveDeleteTask( const Task& ) = 0;

    virtual void commitCommand( CharmCommand* ) = 0;
};

#endif
