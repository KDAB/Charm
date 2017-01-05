/*
  MonthlyTimesheetXmlWriter.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2014-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Frank Osterfeld <frank.osterfeld@kdab.com>

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

#include "MonthlyTimesheetXmlWriter.h"

#include "TimesheetInfo.h"
#include "CharmCMake.h"

#include "Core/CharmDataModel.h"
#include <Core/XmlSerialization.h>

#include <QDomDocument>

MonthlyTimesheetXmlWriter::MonthlyTimesheetXmlWriter()
{}

void MonthlyTimesheetXmlWriter::setDataModel( const CharmDataModel* dataModel )
{
    m_dataModel = dataModel;
}

void MonthlyTimesheetXmlWriter::setYearOfMonth( int yearOfMonth )
{
    m_yearOfMonth = yearOfMonth;
}

void MonthlyTimesheetXmlWriter::setMonthNumber( int monthNumber )
{
    m_monthNumber = monthNumber;
}

void MonthlyTimesheetXmlWriter::setEvents( const EventList& events )
{
    m_events = events;
}

void MonthlyTimesheetXmlWriter::setNumberOfWeeks( int numberOfWeeks )
{
    m_numberOfWeeks = numberOfWeeks;
}

void MonthlyTimesheetXmlWriter::setRootTask( TaskId rootTask )
{
    m_rootTask = rootTask;
}

QByteArray MonthlyTimesheetXmlWriter::saveToXml() const
{
    QDomDocument document = XmlSerialization::createXmlTemplate( QStringLiteral("monthly-timesheet") );

    // find metadata and report element:
    QDomElement root = document.documentElement();
    QDomElement metadata = XmlSerialization::metadataElement( document );
    QDomElement charmVersion = document.createElement( QStringLiteral("charmversion") );
    QDomText charmVersionString = document.createTextNode( QStringLiteral(CHARM_VERSION) );
    charmVersion.appendChild( charmVersionString );
    metadata.appendChild( charmVersion );
    QDomElement report = XmlSerialization::reportElement( document );
    Q_ASSERT( !root.isNull() && !metadata.isNull() && !report.isNull() );

    // extend metadata tag: add year, and serial (month) number:
    {
        QDomElement yearElement = document.createElement( QStringLiteral("year") );
        metadata.appendChild( yearElement );
        QDomText text = document.createTextNode( QString::number( m_yearOfMonth ) );
        yearElement.appendChild( text );
        QDomElement monthElement = document.createElement( QStringLiteral("serial-number") );
        monthElement.setAttribute( QStringLiteral("semantics"), QStringLiteral("month-number") );
        metadata.appendChild( monthElement );
        QDomText monthtext = document.createTextNode( QString::number( m_monthNumber ) );
        monthElement.appendChild( monthtext );
    }

    typedef QMap< TaskId, QVector<int> > SecondsMap;
    SecondsMap secondsMap;
    TimeSheetInfoList timeSheetInfo = TimeSheetInfo::filteredTaskWithSubTasks(
        TimeSheetInfo::taskWithSubTasks( m_dataModel, m_numberOfWeeks, m_rootTask, secondsMap ),
        false ); // here, we don't care about active or not, because we only report on the tasks

    // extend report tag: add tasks and effort structure
    {   // tasks
        QDomElement tasks = document.createElement( QStringLiteral("tasks") );
        report.appendChild( tasks );
        Q_FOREACH ( const TimeSheetInfo& info, timeSheetInfo ) {
            if ( info.taskId == 0 ) // the root task
                continue;
            const Task& modelTask = m_dataModel->getTask( info.taskId );
            tasks.appendChild( modelTask.toXml( document ) );
        }
    }
    {   // effort
        // make effort element:
        QDomElement effort = document.createElement( QStringLiteral("effort") );
        report.appendChild( effort );

        // aggregate (group by task and day):
        typedef QPair<TaskId, QDate> Key;
        QMap< Key, Event> events;
        Q_FOREACH ( const Event& event, m_events ) {
            TimeSheetInfoList::iterator it;
            for ( it = timeSheetInfo.begin(); it != timeSheetInfo.end(); ++it )
                if ( ( *it ).taskId == event.taskId() ) break;
            if ( it == timeSheetInfo.end() )
                continue;
            Key key( event.taskId(), event.startDateTime().date() );
            if ( events.contains( key ) ) {
                // add to previous events:
                const Event& oldEvent = events[key];
                const int seconds = oldEvent.duration() + event.duration();
                const QDateTime start = oldEvent.startDateTime();
                const QDateTime end( start.addSecs( seconds ) );
                Q_ASSERT( start.secsTo( end ) == seconds );
                Event newEvent( oldEvent );
                newEvent.setStartDateTime( start );
                newEvent.setEndDateTime( end );
                Q_ASSERT( newEvent.duration() == seconds );
                QString comment = oldEvent.comment();
                if ( ! event.comment().isEmpty() ) {
                    if ( !comment.isEmpty() ) { // make separator
                        comment += QLatin1String(" / ");
                    }
                    comment += event.comment();
                    newEvent.setComment( comment );
                }
                events[key] = newEvent;
            } else {
                // add this event:
                events[key] = event;
                events[key].setId( -events[key].id() ); // "synthetic" :-)
                // move to start at midnight in UTC (for privacy reasons)
                // never, never, never use setTime() here, it breaks on DST changes! (twice a year)
                QDateTime start( event.startDateTime().date(), QTime(0, 0, 0, 0), Qt::UTC );
                QDateTime end( start.addSecs( event.duration() ) );
                events[key].setStartDateTime( start );
                events[key].setEndDateTime( end );
                Q_ASSERT( events[key].duration() == event.duration() );
                Q_ASSERT( start.time() == QTime(0, 0, 0, 0) );
            }
        }
        // create elements:
        Q_FOREACH ( const Event & event, events ) {
            effort.appendChild( event.toXml( document ) );
        }
    }

   return document.toByteArray( 4 );
}
