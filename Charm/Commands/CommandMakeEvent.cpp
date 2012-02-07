#include <QtDebug>
#include <QDateTime>

#include "EventView.h"
#include <Core/ControllerInterface.h>

#include "CommandMakeEvent.h"

CommandMakeEvent::CommandMakeEvent( const Task& task,
                                    QObject* parent )
    : CharmCommand( parent )
    , m_task( task )
{
}

CommandMakeEvent::CommandMakeEvent( const Event& event,
                                    QObject* parent )
    : CharmCommand( parent )
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

bool CommandMakeEvent::finalize()
{
    if ( m_event.isValid() ) {
        EventView* view = dynamic_cast<EventView*>( owner() );
        Q_ASSERT( view ); // this command is "owned" by the event view
        view->makeVisibleAndCurrent( m_event );
        emit finishedOk( m_event );
        return true;
    } else {
        return false;
    }
}

#include "CommandMakeEvent.moc"
