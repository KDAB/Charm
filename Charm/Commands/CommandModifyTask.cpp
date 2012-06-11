#include <QWidget>
#include <QMessageBox>

#include <Core/ControllerInterface.h>
#include <Core/CommandEmitterInterface.h>

#include "CommandModifyTask.h"

CommandModifyTask::CommandModifyTask( const Task& task, QObject* parent )
    : CharmCommand( tr("Edit Task"), parent )
    , m_task( task )
    , m_success( false )
{
}

CommandModifyTask::~CommandModifyTask()
{
}

bool CommandModifyTask::prepare()
{
    return true;
}

bool CommandModifyTask::execute( ControllerInterface* controller )
{
    m_success = controller->modifyTask( m_task );
    return m_success;
}

bool CommandModifyTask::finalize()
{
    if ( !m_success ) {
        QWidget* parent = dynamic_cast<QWidget*>( owner() );
        Q_ASSERT( parent );
        // this might be slightly to little informative:
        QMessageBox::information( parent, tr( "Unable to modify task" ),
                                  tr( "Modifying the task failed." ) );
    }
    return m_success;
}

#include "CommandModifyTask.moc"

