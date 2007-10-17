#ifndef MODELINTERFACE_H
#define MODELINTERFACE_H

class QModelIndex;

#include "Task.h"
#include "Event.h"

class TaskModelInterface
{
public:
    virtual ~TaskModelInterface() {}
    virtual Task taskForIndex( const QModelIndex& ) const = 0;
    virtual QModelIndex indexForTaskId( TaskId ) const = 0;
    virtual bool taskIsActive( const Task& task ) const = 0;
    virtual bool taskHasChildren( const Task& task ) const = 0;
    virtual bool taskIdExists( TaskId taskId ) const = 0;
    // relayed model signals, in lack of notification in the view:
    // eventActivated was already taken by CharmDataModelAdapterInterface
    virtual void eventActivationNotice( EventId id ) = 0;
    virtual void eventDeactivationNotice( EventId id ) = 0;
};

#endif
