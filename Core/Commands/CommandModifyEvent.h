#ifndef COMMANDMODIFYEVENT_H
#define COMMANDMODIFYEVENT_H

#include "Event.h"
#include "CharmCommand.h"

class CommandModifyEvent : public CharmCommand
{
    Q_OBJECT

public:
    explicit CommandModifyEvent( const Event&, QObject* parent = 0 );
    ~CommandModifyEvent();

    bool prepare();
    bool execute( ControllerInterface* );
    bool finalize();

private:
    Event m_event;
};

#endif
