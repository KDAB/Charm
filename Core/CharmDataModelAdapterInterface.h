#ifndef CHARMDATAMODELADAPTERINTERFACE_H
#define CHARMDATAMODELADAPTERINTERFACE_H

#include "Task.h"
#include "Event.h"
#include <QList>

class CharmDataModelAdapterInterface
{
public:
    // keep compiler happy:
    virtual ~CharmDataModelAdapterInterface() {}

    virtual void resetTasks() = 0;
    virtual void taskAboutToBeAdded( TaskId parent, int pos ) = 0;
    virtual void taskAdded( TaskId id ) = 0;
    virtual void taskModified( TaskId id ) = 0;
    virtual void taskAboutToBeDeleted( TaskId ) = 0;
    virtual void taskDeleted( TaskId id ) = 0;

    virtual void resetEvents() = 0;
    virtual void eventAboutToBeAdded( EventId id ) = 0;
    virtual void eventAdded( EventId id ) = 0;
    // we only pass an event because it is an outdated object:
    virtual void eventModified( EventId id, Event discardedEvent ) = 0;
    virtual void eventAboutToBeDeleted( EventId id ) = 0;
    virtual void eventDeleted( EventId id ) = 0;

    virtual void eventActivated( EventId id ) = 0;
    virtual void eventDeactivated( EventId id ) = 0;
};

typedef QList<CharmDataModelAdapterInterface*> CharmDataModelAdapterList;

#endif
