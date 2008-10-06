#ifndef COMMANDSETALLTASKS_H
#define COMMANDSETALLTASKS_H

#include <Core/Task.h>
#include <Core/CharmCommand.h>

class CommandSetAllTasks : public CharmCommand
{
    Q_OBJECT

public:
    explicit CommandSetAllTasks( const TaskList&, QObject* parent );
    ~CommandSetAllTasks();

    bool prepare();
    bool execute( ControllerInterface* );
    bool finalize();

private:
    TaskList m_tasks;
    bool m_success;
};

#endif
