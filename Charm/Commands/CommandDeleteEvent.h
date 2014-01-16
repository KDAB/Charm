#ifndef COMMANDDELETEEVENT_H
#define COMMANDDELETEEVENT_H

#include <Core/Event.h>
#include <Core/CharmCommand.h>

class CommandDeleteEvent : public CharmCommand
{
    Q_OBJECT

public:
    explicit CommandDeleteEvent( const Event&, QObject* parent = 0 );
    ~CommandDeleteEvent();

    bool prepare();
    bool execute( ControllerInterface* );
    bool rollback( ControllerInterface* );
    bool finalize();

public slots:
    virtual void eventIdChanged(int,int);

private:
    Event m_event;
};

#endif
