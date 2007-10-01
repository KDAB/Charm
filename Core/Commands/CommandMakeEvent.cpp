#include <QtDebug>
#include <QDateTime>

#include "EventEditor.h"
#include "ControllerInterface.h"
#include "CommandMakeEvent.h"

CommandMakeEvent::CommandMakeEvent( const Task& task,
                                    QObject* parent )
    : CharmCommand( parent )
    , m_task( task )
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
    qDebug() << "CommandMakeEvent::execute: creating event";
    Event event = controller->makeEvent( m_task );
    if ( event.isValid() ) {
        QDateTime start( QDateTime::currentDateTime() );
        event.setComment( tr( "(event created in event editor)" ) );
        event.setStartDateTime( start );
        event.setEndDateTime( start.addMSecs( 1000 * 60 * 5 ) );
        if ( controller->modifyEvent( event ) ) {
            m_event = event;
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

bool CommandMakeEvent::finalize()
{
    qDebug() << "CommandMakeEvent::finalize: activating event";
    if ( m_event.isValid() ) {
        EventEditor* editor = dynamic_cast<EventEditor*>( owner() );
        Q_ASSERT( editor ); // this command is "owned" by the editor
        editor->makeVisibleAndCurrent( m_event );
        return true;
    } else {
        return false;
    }
}

#include "CommandMakeEvent.moc"
