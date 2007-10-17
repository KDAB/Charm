#ifndef COMMANDMODIFYTASK_H
#define COMMANDMODIFYTASK_H

#include <Core/Task.h>
#include <Core/CharmCommand.h>

class CommandModifyTask : public CharmCommand
{
    Q_OBJECT

public:
    explicit CommandModifyTask( const Task&, QObject* parent = 0 );
    ~CommandModifyTask();

    bool prepare();
    bool execute( ControllerInterface* );
    bool finalize();

private:
    Task m_task;
    bool m_success;
};

#endif
