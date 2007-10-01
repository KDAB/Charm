#include <QMessageBox>

#include "Charm.h"
#include "CommandDeleteTask.h"
#include "ControllerInterface.h"
#include "CommandEmitterInterface.h"

CommandDeleteTask::CommandDeleteTask( const Task& task, QObject* parent )
    : CharmCommand( parent )
    , m_task( task )
    , m_success( false )
{
}

CommandDeleteTask::~CommandDeleteTask()
{
}

bool CommandDeleteTask::prepare()
{
    return true;
}

bool CommandDeleteTask::execute( ControllerInterface* controller )
{
    m_success = controller->deleteTask( m_task );
    return m_success;
}

bool CommandDeleteTask::finalize()
{
    if ( !m_success ) {
        QMessageBox::information( &VIEW, tr( "Unable to delete task" ),
                                  tr( "Deleting the task failed" ) );
    }
    return m_success;
}

#include "CommandDeleteTask.moc"
