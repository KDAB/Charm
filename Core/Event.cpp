#include <QDomElement>
#include <QDomText>

#include "Event.h"

Event::Event()
    : m_installationId( 0 )
    , m_id()
    , m_taskId()
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
{   // negative values are allowed and indicate calculated values
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
    // strip milliseconds, this is necessary for the precision of serialization:
    QTime time( m_start.time() );
    time.setHMS( time.hour(), time.minute(), time.second() );
    m_start.setTime( time );
}

const QDateTime& Event::endDateTime() const
{
    return m_end;
}

void Event::setEndDateTime( const QDateTime& end )
{
    m_end = end;
    // strip milliseconds, this is necessary for the precision of serialization:
    QTime time( m_end.time() );
    time.setHMS( time.hour(), time.minute(), time.second() );
    m_end.setTime( time );
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

const QString EventElement( "event" );
const QString EventIdAttribute( "eventid" );
const QString EventInstallationIdAttribute( "installationid" );
const QString EventTaskIdAttribute( "taskid" );
const QString EventStartAttribute( "start" );
const QString EventEndAttribute( "end" );

QDomElement Event::toXml( QDomDocument document ) const
{
    QDomElement element = document.createElement( EventElement );
    element.setAttribute( EventIdAttribute, QString().setNum( id() ) );
    element.setAttribute( EventInstallationIdAttribute, QString().setNum( installationId() ) );
    element.setAttribute( EventTaskIdAttribute, QString().setNum( taskId() ) );
    if ( m_start.isValid() ) {
        element.setAttribute( EventStartAttribute, m_start.toString( Qt::ISODate ) );
    }
    if ( m_end.isValid() ) {
        element.setAttribute( EventEndAttribute, m_end.toString( Qt::ISODate ) );
    }
    if ( !comment().isEmpty() ) {
        QDomText commentText = document.createTextNode( comment() );
        element.appendChild( commentText );
    }
    return element;
}

Event Event::fromXml( const QDomElement& element ) throw ( Event::XmlSerializationException )
{   // in case any event object creates trouble with
    // serialization/deserialization, add an object of it to
    // void XmlSerializationTests::testEventSerialization()
    Event event;
    bool ok;
    event.setComment( element.text() );
    event.setId( element.attribute( EventIdAttribute ).toInt( &ok ) );
    if ( !ok ) throw Event::XmlSerializationException( "Event::fromXml: invalid event id" );
    event.setInstallationId( element.attribute( EventInstallationIdAttribute ).toInt( &ok ) );
    if ( !ok ) throw Event::XmlSerializationException( "Event::fromXml: invalid installation id" );
    event.setTaskId( element.attribute( EventTaskIdAttribute ).toInt( &ok ) );
    if ( !ok ) throw Event::XmlSerializationException( "Event::fromXml: invalid task id" );
    if ( element.hasAttribute( EventStartAttribute ) ) {
        QDateTime start = QDateTime::fromString( element.attribute( EventStartAttribute ), Qt::ISODate );
        if ( !start.isValid() ) throw Event::XmlSerializationException( "Event::fromXml: invalid start date" );
        event.setStartDateTime( start );
    }
    if ( element.hasAttribute( EventEndAttribute ) ) {
        QDateTime end = QDateTime::fromString( element.attribute( EventEndAttribute ), Qt::ISODate );
        if ( !end.isValid() ) throw Event::XmlSerializationException( "Event::fromXml: invalid end date" );
        event.setEndDateTime( end );
    }
    event.setComment( element.text() );
    return event;
}
