#include <QDateTime>

#include <Core/CharmDataModel.h>
#include <Core/CommandEmitterInterface.h>
#include <Core/ControllerInterface.h>

#include "ModelConnector.h"
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
        ModelConnector* model = dynamic_cast<ModelConnector*>( owner() );
        Q_ASSERT( model ); // this command is "owned" by the model
        model->charmDataModel()->activateEvent( m_event );
        return true;
    } else {
        return false;
    }
}

#include "CommandMakeAndActivateEvent.moc"
