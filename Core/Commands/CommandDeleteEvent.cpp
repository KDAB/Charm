#include "CommandDeleteEvent.h"
#include "ControllerInterface.h"

CommandDeleteEvent::CommandDeleteEvent( const Event& event, QObject* parent )
    : CharmCommand( parent )
    , m_event( event )
{
}

CommandDeleteEvent::~CommandDeleteEvent()
{
}

bool CommandDeleteEvent::prepare()
{
    return true;
}

bool CommandDeleteEvent::execute( ControllerInterface* controller )
{
    qDebug() << "CommandDeleteEvent::execute: deleting:";
    m_event.dump();
    return controller->deleteEvent( m_event );
}

bool CommandDeleteEvent::finalize()
{
    return true;
}

#include "CommandDeleteEvent.moc"

