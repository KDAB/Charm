#ifndef COMMANDDELETETASK_H
#define COMMANDDELETETASK_H

#include "../Task.h"
#include "CharmCommand.h"

class CommandDeleteTask : public CharmCommand
{
    Q_OBJECT

public:
    explicit CommandDeleteTask( const Task&, QObject* parent = 0 );
    ~CommandDeleteTask();

    bool prepare();
    bool execute( ControllerInterface* );
    bool finalize();

private:
    Task m_task;
    bool m_success;
};

#endif
