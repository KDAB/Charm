#include <QWidget>
#include <QMessageBox>

#include <Core/ControllerInterface.h>
#include <Core/CommandEmitterInterface.h>

#include "CommandSetAllTasks.h"

CommandSetAllTasks::CommandSetAllTasks( const TaskList& tasks, QObject* parent )
    : CharmCommand( parent )
    , m_tasks( tasks )
    , m_success( false )
{
}

CommandSetAllTasks::~CommandSetAllTasks()
{
}

bool CommandSetAllTasks::prepare()
{
    return true;
}

bool CommandSetAllTasks::execute( ControllerInterface* controller )
{
    m_success = controller->setAllTasks( m_tasks );
    return m_success;
}

bool CommandSetAllTasks::finalize()
{
    if ( m_success ) {
        QWidget* parent = dynamic_cast<QWidget*>( owner() );
        Q_ASSERT( parent );
        QMessageBox::information( parent, tr( "New Tasks Imported" ),
                                  tr( "The new tasks have been successfully imported." ) );
    } else {
        QWidget* parent = dynamic_cast<QWidget*>( owner() );
        Q_ASSERT( parent );
        QMessageBox::information( parent, tr( "Failure setting new tasks" ),
                                  tr( "Setting the new tasks failed." ) );
    }
    return m_success;
}

#include "CommandSetAllTasks.moc"

