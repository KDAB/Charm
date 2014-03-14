#include <Core/CharmConstants.h>
#include <Core/ControllerInterface.h>
#include <Core/CommandEmitterInterface.h>

#include "ViewHelpers.h"
#include "CommandDeleteTask.h"

CommandDeleteTask::CommandDeleteTask( const Task& task, QObject* parent )
    : CharmCommand( tr("Delete Task"), parent )
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
        showInformation( tr( "Unable to delete task" ), tr( "Deleting the task failed" ) );
    }
    return m_success;
}

#include "moc_CommandDeleteTask.cpp"
