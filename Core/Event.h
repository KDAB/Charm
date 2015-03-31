/*
  Event.h

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

#ifndef CHARM_EVENT_H
#define CHARM_EVENT_H

#include <map>

#include <QList>
#include <QtDebug>
#include <QDateTime>
#include <QDomElement>
#include <QDomDocument>

#include "Task.h"

typedef int EventId;

/** An event is a recorded time for a task.
    It records the according task, the duration and a possible
    comment. */
class Event {
public:
    Event();

    bool operator == ( const Event& other ) const;

    bool operator != ( const Event& other ) const
    { return ! operator==( other ); }

    EventId id() const;

    void setId( EventId id );

    int userId() const;

    void setUserId( int userId );

    int reportId() const;

    void setReportId( int userId );

    void setInstallationId( int instId );

    int installationId() const;

    bool isValid() const;

    TaskId taskId() const;

    void setTaskId( TaskId id);

    QString comment() const;

    void setComment( const QString& );

    QDateTime startDateTime( Qt::TimeSpec timeSpec = Qt::LocalTime ) const;

    void setStartDateTime( const QDateTime& start = QDateTime::currentDateTime() );

    QDateTime endDateTime( Qt::TimeSpec timeSpec = Qt::LocalTime ) const;

    void setEndDateTime( const QDateTime& end = QDateTime::currentDateTime() );

    /** Returns the duration of this event in seconds. */
    int duration () const;

    void dump() const;

    QDomElement toXml( QDomDocument ) const;

    static Event fromXml( const QDomElement&,  int databaseSchemaVersion = 1 );
    static QString tagName();

private:
    /** The id of the user who owns the event. */
    int m_userid;
    /** The report id. This field is only useful
     * if the event is imported from a report.
     */
    int m_reportid;
    /** The installation-unique id of the event. */
    int m_installationId;
    int m_id;
    /** The task this event belongs to. */
    TaskId m_taskId;
    /** A possible user comment.
        May be empty. */
    QString m_comment;
    /** The start datetime of the event. */
    QDateTime m_start;
    /** The end datetime of the event. */
    QDateTime m_end;
};

/** A list of events. */
typedef QList<Event> EventList;

/** A list of event ids. */
typedef QList<EventId> EventIdList;

/** A map of events. */
typedef std::map<EventId, Event> EventMap;

void dumpEvents( const EventList& events );

#endif
