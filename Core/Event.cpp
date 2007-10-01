#include "Event.h"

Event::Event()
    : m_installationId( 0 )
    , m_id( 0 )
{
}

bool Event::operator == ( const Event& other ) const
{
    return ( other.id() == id()
             && other.installationId() == installationId()
             && other.taskId() == taskId()
             && other.comment() == comment()
             && other.startDateTime() ==  startDateTime()
             && other.endDateTime() == endDateTime() );
}

EventId Event::id() const
{
    return m_id;
}

void Event::setId( EventId id )
{
    m_id = id;
}

void Event::setInstallationId( int instId )
{
    m_installationId = instId;
}

int Event::installationId() const
{
    return m_installationId;
}

bool Event::isValid() const
{
    return id() != 0 && m_installationId != 0;
}

TaskId Event::taskId() const
{
    return m_taskId;
}

void Event::setTaskId( TaskId taskId )
{
    m_taskId = taskId;
}

const QString& Event::comment() const
{
    return m_comment;
}

void Event::setComment ( const QString& comment )
{
    m_comment = comment;
}

const QDateTime& Event::startDateTime() const
{
    return m_start;
}

void Event::setStartDateTime( const QDateTime& start )
{
    m_start = start;
}

const QDateTime& Event::endDateTime() const
{
    return m_end;
}

void Event::setEndDateTime( const QDateTime& end )
{
    m_end = end;
}





int Event::duration() const
{
    if ( m_start.isValid() && m_end.isValid() )
        return m_start.secsTo( m_end );
    else
        return 0;
}

void Event::dump() const
{
    qDebug() << "[Event" << id() << "] - task "
             << taskId()
             << " - start: " << startDateTime()
             << " - end: " << endDateTime()
             << " - duration: " << duration()
             << "seconds - comment:" << comment();
}

void dumpEvents( const EventList& events )
{
    for ( int i = 0; i < events.size(); ++i )
        events[i].dump();
}


