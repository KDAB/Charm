#ifndef COMMANDMAKEEVENT_H
#define COMMANDMAKEEVENT_H

#include <Core/Task.h>
#include <Core/Event.h>
#include <Core/CharmCommand.h>

class QObject;

class CommandMakeEvent : public CharmCommand
{
    Q_OBJECT

public:
    explicit CommandMakeEvent( const Task& task, QObject* parent );
    explicit CommandMakeEvent( const Event& event, QObject* parent );
    ~CommandMakeEvent();

    bool prepare();
    bool execute( ControllerInterface* );
    bool rollback( ControllerInterface* );
    bool finalize();

public slots:
    virtual void eventIdChanged(int,int);

Q_SIGNALS:
    void finishedOk( const Event& );

private:
    bool m_rollback; //don't show the event in finalize
    Task m_task; // the task the new event should be assigned to
    Event m_event; // the result, only valid after the event has been created
};

#endif
