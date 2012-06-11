#include <Core/ControllerInterface.h>
#include <Core/StorageInterface.h>

#include "CommandModifyEvent.h"

CommandModifyEvent::CommandModifyEvent( const Event& event, const Event& oldEvent, QObject* parent )
    : CharmCommand( tr("Modify Event"), parent )
    , m_event( event )
    , m_oldEvent( oldEvent )
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

bool CommandModifyEvent::rollback(ControllerInterface *controller)
{
    return controller->modifyEvent( m_oldEvent );
}

bool CommandModifyEvent::finalize()
{
    return true;
}

void CommandModifyEvent::eventIdChanged(int oid, int nid)
{
    if(m_event.id() == oid)
    {
        m_event.setId(nid);
        m_oldEvent.setId(nid);
    }
}

#include "CommandModifyEvent.moc"


