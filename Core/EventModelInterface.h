
#ifndef EVENTMODELINTERFACE_H
#define EVENTMODELINTERFACE_H

class Event;
class QModelIndex;

class EventModelInterface
{
public:
    virtual ~EventModelInterface() {}

    virtual const Event& eventForIndex( const QModelIndex& ) const = 0;
    virtual QModelIndex indexForEvent( const Event& ) const = 0;
};

#endif
