#ifndef COMMANDMODIFYEVENT_H
#define COMMANDMODIFYEVENT_H

#include <Core/Event.h>
#include <Core/CharmCommand.h>

class CommandModifyEvent : public CharmCommand
{
    Q_OBJECT

public:
    explicit CommandModifyEvent( const Event&, const Event&, QObject* parent = nullptr );
    ~CommandModifyEvent();

    bool prepare();
    bool execute( ControllerInterface* );
    bool rollback( ControllerInterface* );
    bool finalize();

public slots:
    virtual void eventIdChanged(int,int);

private:
    Event m_event;
    Event m_oldEvent;
};

#endif
