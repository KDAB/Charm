#include <QDateTime>

#include "CharmDataModel.h"
#include "ControllerInterface.h"
#include "CommandMakeAndActivateEvent.h"

CommandMakeAndActivateEvent::CommandMakeAndActivateEvent( const Task& task,
                                                          QObject* parent )
    : CharmCommand( parent )
    , m_task( task )
{
}

CommandMakeAndActivateEvent::~CommandMakeAndActivateEvent()
{
}

bool CommandMakeAndActivateEvent::prepare()
{
    return true;
}

bool CommandMakeAndActivateEvent::execute( ControllerInterface* controller )
{
    m_event = controller->makeEvent( m_task );
    if ( m_event.isValid() ) {
        m_event.setTaskId( m_task.id() );
        m_event.setStartDateTime( QDateTime::currentDateTime() );
        return controller->modifyEvent( m_event );
    } else {
        return false;
    }
}

bool CommandMakeAndActivateEvent::finalize()
{
    if ( m_event.isValid() ) {
        CharmDataModel* model = dynamic_cast<CharmDataModel*>( owner() );
        Q_ASSERT( model ); // this command is "owned" by the model
        model->activateEvent( m_event );
        return true;
    } else {
        return false;
    }
}

#include "CommandMakeAndActivateEvent.moc"
