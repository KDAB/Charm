#include <QtDebug>
#include <QDateTime>

#include "Widgets/EventView.h"
#include <Core/ControllerInterface.h>

#include "CommandMakeEvent.h"

CommandMakeEvent::CommandMakeEvent( const Task& task,
                                    QObject* parent )
    : CharmCommand( tr("Create Event"), parent )
    , m_task( task )
{
}

CommandMakeEvent::CommandMakeEvent( const Event& event,
                                    QObject* parent )
    : CharmCommand( tr("Create Event"), parent )
    , m_event( event )
{
}



CommandMakeEvent::~CommandMakeEvent()
{
}

bool CommandMakeEvent::prepare()
{
    return true;
}

bool CommandMakeEvent::execute( ControllerInterface* controller )
{
    m_rollback = false;

    if(m_event.id()) //if it already has an id, this is a redo operation
    {
        int oid = m_event.id();
        m_event = controller->cloneEvent(m_event);
        int nid = m_event.id();
        if(oid != nid)
            emit emitSlotEventIdChanged(oid, nid);
        return m_event.isValid();
    }

    Event event = controller->makeEvent( m_task );
    if ( !event.isValid() )
         return false;

    QDateTime start( QDateTime::currentDateTime() );
    event.setStartDateTime( start );
    event.setEndDateTime( start );

    if ( m_event.startDateTime().isValid() )
        event.setStartDateTime( m_event.startDateTime() );
    if ( m_event.endDateTime().isValid() )
        event.setEndDateTime( m_event.endDateTime() );
    if ( !m_event.comment().isEmpty() )
        event.setComment( m_event.comment() );
    if ( m_event.taskId() != 0 )
        event.setTaskId( m_event.taskId() );

    if ( controller->modifyEvent( event ) ) {
        m_event = event;
        return true;
    } else {
        return false;
    }
}

bool CommandMakeEvent::rollback(ControllerInterface *controller )
{
    m_rollback = true;
    return controller->deleteEvent(m_event);
}

bool CommandMakeEvent::finalize()
{
    if ( m_rollback )
        return false;
    if ( m_event.isValid() ) {
        EventView* view = dynamic_cast<EventView*>( owner() );
        if ( view )
            view->makeVisibleAndCurrent( m_event );
        emit finishedOk( m_event );
        return true;
    } else {
        return false;
    }
}

void CommandMakeEvent::eventIdChanged(int oid, int nid)
{
    if(m_event.id() == oid)
        m_event.setId(nid);
}

#include "moc_CommandMakeEvent.cpp"
