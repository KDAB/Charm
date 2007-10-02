#ifndef COMMANDMAKEEVENT_H
#define COMMANDMAKEEVENT_H

#include "../Task.h"
#include "../Event.h"
#include "CharmCommand.h"

class QObject;

class CommandMakeEvent : public CharmCommand
{
    Q_OBJECT

public:
    explicit CommandMakeEvent( const Task& task, QObject* parent );
    ~CommandMakeEvent();

    bool prepare();
    bool execute( ControllerInterface* );
    bool finalize();

private:
    Task m_task; // the task the new event should be assigned to
    Event m_event; // the result, only valid after the event has been created
};

#endif
