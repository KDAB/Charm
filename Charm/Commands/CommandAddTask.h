#ifndef COMMANDADDTASK_H
#define COMMANDADDTASK_H

#include <Core/Task.h>
#include <Core/CharmCommand.h>

class CommandAddTask : public CharmCommand
{
    Q_OBJECT

public:
    explicit CommandAddTask( const Task&, QObject* parent = nullptr );
    ~CommandAddTask();

    bool prepare();
    bool execute( ControllerInterface* );
    bool finalize();

private:
    Task m_task;
    bool m_success;
};

#endif
