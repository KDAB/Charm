#ifndef COMMANDADDTASK_H
#define COMMANDADDTASK_H

#include "../Task.h"
#include "CharmCommand.h"

class CommandAddTask : public CharmCommand
{
    Q_OBJECT

public:
    explicit CommandAddTask( const Task&, QObject* parent = 0 );
    ~CommandAddTask();

    bool prepare();
    bool execute( ControllerInterface* );
    bool finalize();

private:
    Task m_task;
    bool m_success;
};

#endif
