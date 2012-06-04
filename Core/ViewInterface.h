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
    virtual void visibilityChanged( bool ) = 0; // implement as signal and emit from show and hide events
    virtual void configurationChanged() = 0;
    virtual void saveConfiguration() = 0;

    virtual void emitCommand( CharmCommand* ) = 0;
    virtual void emitCommandRollback( CharmCommand* ) = 0;
    virtual void sendCommand( CharmCommand* ) = 0;
    virtual void sendCommandRollback( CharmCommand* ) = 0;
    virtual void commitCommand( CharmCommand* ) = 0;

    // restore the view
    virtual void restore() = 0;
    // quit the application
    virtual void quit() = 0;

};

#endif
