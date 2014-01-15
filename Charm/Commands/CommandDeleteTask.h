#ifndef COMMANDDELETETASK_H
#define COMMANDDELETETASK_H

#include <Core/Task.h>
#include <Core/CharmCommand.h>

class CommandDeleteTask : public CharmCommand
{
    Q_OBJECT

public:
    explicit CommandDeleteTask( const Task&, QObject* parent = nullptr );
    ~CommandDeleteTask();

    bool prepare();
    bool execute( ControllerInterface* );
    bool finalize();

private:
    Task m_task;
    bool m_success;
};

#endif
