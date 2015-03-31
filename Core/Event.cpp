/*
  Event.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2007-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Event.h"
#include "CharmExceptions.h"

#include <QDomElement>
#include <QDomText>

Event::Event()
    : m_userid()
    , m_reportid()
    , m_installationId()
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
             && other.endDateTime() == endDateTime()
             && other.userId() == userId()
             && other.reportId() == reportId() );
}

EventId Event::id() const
{
    return m_id;
}

void Event::setId( EventId id )
{
    m_id = id;
}

int Event::userId() const
{
    return m_userid;
}

void Event::setUserId( int userId )
{
    m_userid = userId;
}

int Event::reportId() const
{
    return m_reportid;
}

void Event::setReportId( int reportId )
{
    m_reportid = reportId;
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

QString Event::comment() const
{
    return m_comment;
}

void Event::setComment ( const QString& comment )
{
    m_comment = comment;
}

QDateTime Event::startDateTime( Qt::TimeSpec timeSpec ) const
{
    return m_start.toTimeSpec( timeSpec );
}

void Event::setStartDateTime( const QDateTime& start )
{
    m_start = start.toUTC();
    // strip milliseconds, this is necessary for the precision of serialization:
    m_start = m_start.addMSecs( -m_start.time().msec() );
    Q_ASSERT( qAbs( m_start.secsTo( start ) ) <= 1 );
    Q_ASSERT( m_start.time().msec() == 0 );
}

QDateTime Event::endDateTime( Qt::TimeSpec timeSpec ) const
{
    return m_end.toTimeSpec( timeSpec );
}

void Event::setEndDateTime( const QDateTime& end )
{
    m_end = end.toUTC();
    // strip milliseconds, this is necessary for the precision of serialization:
    m_end = m_end.addMSecs( -m_end.time().msec() );
    Q_ASSERT( qAbs( m_end.secsTo( end ) ) <= 1 );
    Q_ASSERT( m_end.time().msec() == 0 );
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
const QString EventUserIdAttribute( "userid" );
const QString EventReportIdAttribute( "reportid" );
const QString EventStartAttribute( "start" );
const QString EventEndAttribute( "end" );

QDomElement Event::toXml( QDomDocument document ) const
{
    QDomElement element = document.createElement( EventElement );
    element.setAttribute( EventIdAttribute, QString().setNum( id() ) );
    element.setAttribute( EventInstallationIdAttribute, QString().setNum( installationId() ) );
    element.setAttribute( EventTaskIdAttribute, QString().setNum( taskId() ) );
    element.setAttribute( EventUserIdAttribute, QString().setNum( userId() ) );
    element.setAttribute( EventReportIdAttribute, QString().setNum( reportId() ) );
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

QString Event::tagName()
{
    static const QString tag( QString::fromLatin1( "event" ) );
    return tag;
}

Event Event::fromXml( const QDomElement& element, int databaseSchemaVersion )
{   // in case any event object creates trouble with
    // serialization/deserialization, add an object of it to
    // void XmlSerializationTests::testEventSerialization()
    Event event;
    bool ok;
    event.setComment( element.text() );
    event.setId( element.attribute( EventIdAttribute ).toInt( &ok ) );
    if ( !ok ) throw XmlSerializationException( QObject::tr( "Event::fromXml: invalid event id" ) );
    event.setInstallationId( element.attribute( EventInstallationIdAttribute ).toInt( &ok ) );
    if ( !ok ) throw XmlSerializationException( QObject::tr( "Event::fromXml: invalid installation id" ) );
    event.setTaskId( element.attribute( EventTaskIdAttribute ).toInt( &ok ) );
    if ( !ok ) throw XmlSerializationException( QObject::tr( "Event::fromXml: invalid task id" ) );
    event.setUserId( element.attribute( EventUserIdAttribute ).toInt( &ok ) );
    if ( !ok && databaseSchemaVersion > 1 ) {
        throw XmlSerializationException( QObject::tr( "Event::fromXml: invalid user id" ) );
        event.setUserId( 0 );
    }
    event.setReportId( element.attribute( EventReportIdAttribute ).toInt( &ok ) );
    if ( !ok && databaseSchemaVersion > 1 ) {
        throw XmlSerializationException( QObject::tr( "Event::fromXml: invalid report id" ) );
        event.setReportId( 0 );
    }
    if ( element.hasAttribute( EventStartAttribute ) ) {
        QDateTime start = QDateTime::fromString( element.attribute( EventStartAttribute ), Qt::ISODate );
        if ( !start.isValid() ) throw XmlSerializationException( QObject::tr( "Event::fromXml: invalid start date" ) );
        start.setTimeSpec( Qt::UTC );
        event.setStartDateTime( start );
    }
    if ( element.hasAttribute( EventEndAttribute ) ) {
        QDateTime end = QDateTime::fromString( element.attribute( EventEndAttribute ), Qt::ISODate );
        if ( !end.isValid() ) throw XmlSerializationException( QObject::tr( "Event::fromXml: invalid end date" ) );
        end.setTimeSpec( Qt::UTC );
        event.setEndDateTime( end.toLocalTime() );
    }
    event.setComment( element.text() );
    return event;
}
