#include <QWidget>
#include <QMessageBox>

#include "CommandAddTask.h"
#include "ControllerInterface.h"
#include "CommandEmitterInterface.h"

CommandAddTask::CommandAddTask( const Task& task, QObject* parent )
    : CharmCommand( parent )
    , m_task( task )
    , m_success( false )
{
}

CommandAddTask::~CommandAddTask()
{
}

bool CommandAddTask::prepare()
{
    return true;
}

bool CommandAddTask::execute( ControllerInterface* controller )
{
    m_success = controller->addTask( m_task );
    return m_success;
}

bool CommandAddTask::finalize()
{
    if ( !m_success ) {
        QWidget* parent = dynamic_cast<QWidget*>( owner() );
        Q_ASSERT( parent );
        QMessageBox::information( parent, tr( "Unable to add task" ),
                                  tr( "Adding the task failed." ) );
    }
    return m_success;
}

#include "CommandAddTask.moc"



