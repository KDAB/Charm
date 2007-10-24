#include <Core/ControllerInterface.h>
#include <Core/StorageInterface.h>

#include "CommandModifyEvent.h"

CommandModifyEvent::CommandModifyEvent( const Event& event, QObject* parent )
    : CharmCommand( parent )
    , m_event( event )
{
}

CommandModifyEvent::~CommandModifyEvent()
{
}

bool CommandModifyEvent::prepare()
{
    return true;
}

bool CommandModifyEvent::execute( ControllerInterface* controller )
{
//     qDebug() << "CommandModifyEvent::execute: committing:";
//     m_event.dump();
    return controller->modifyEvent( m_event );
}

bool CommandModifyEvent::finalize()
{
    return true;
}

#include "CommandModifyEvent.moc"


