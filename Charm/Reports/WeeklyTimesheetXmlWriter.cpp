/*
  WeeklyTimesheetXmlWriter.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

#include "WeeklyTimesheetXmlWriter.h"
#include "TimesheetInfo.h"
#include "CharmCMake.h"

#include "Core/CharmDataModel.h"
#include <Core/XmlSerialization.h>

#include <QDomDocument>

static const int DaysInWeek = 7;

WeeklyTimesheetXmlWriter::WeeklyTimesheetXmlWriter()
    : m_dataModel( nullptr )
    , m_year( 0 )
    , m_weekNumber( 0 )
    , m_rootTask()
{
}

void WeeklyTimesheetXmlWriter::setDataModel( const CharmDataModel* model )
{
    m_dataModel = model;
}

void WeeklyTimesheetXmlWriter::setYear( int year )
{
    m_year = year;
}

void WeeklyTimesheetXmlWriter::setWeekNumber( int weekNumber )
{
    m_weekNumber = weekNumber;
}

void WeeklyTimesheetXmlWriter::setEvents( const EventList& events )
{
    m_events = events;
}

void WeeklyTimesheetXmlWriter::setRootTask( TaskId rootTask )
{
    m_rootTask = rootTask;
}

QByteArray WeeklyTimesheetXmlWriter::saveToXml() const
{
    // now create the report:
    QDomDocument document = XmlSerialization::createXmlTemplate( "weekly-timesheet" );

    // find metadata and report element:
    QDomElement root = document.documentElement();
    QDomElement metadata = XmlSerialization::metadataElement( document );
    QDomElement charmVersion = document.createElement( "charmversion" );
    QDomText charmVersionString = document.createTextNode( CHARM_VERSION );
    charmVersion.appendChild( charmVersionString );
    metadata.appendChild( charmVersion );
    QDomElement report = XmlSerialization::reportElement( document );
    Q_ASSERT( !root.isNull() && !metadata.isNull() && !report.isNull() );

    // extend metadata tag: add year, and serial (week) number:
    {
        QDomElement yearElement = document.createElement( "year" );
        metadata.appendChild( yearElement );
        QDomText text = document.createTextNode( QString::number( m_year ) );
        yearElement.appendChild( text );
        QDomElement weekElement = document.createElement( "serial-number" );
        weekElement.setAttribute( "semantics", "week-number" );
        metadata.appendChild( weekElement );
        QDomText weektext = document.createTextNode( QString::number( m_weekNumber ) );
        weekElement.appendChild( weektext );
    }


    typedef QMap< TaskId, QVector<int> > SecondsMap;
    SecondsMap secondsMap;
    TimeSheetInfoList timeSheetInfo = TimeSheetInfo::filteredTaskWithSubTasks(
        TimeSheetInfo::taskWithSubTasks( m_dataModel, DaysInWeek, m_rootTask, secondsMap ),
        false ); // here, we don't care about active or not, because we only report on the tasks

    // extend report tag: add tasks and effort structure
    {   // tasks
        QDomElement tasks = document.createElement( "tasks" );
        report.appendChild( tasks );
        Q_FOREACH ( const TimeSheetInfo& info, timeSheetInfo ) {
            if ( info.taskId == 0 ) // the root task
                continue;
            const Task& modelTask = m_dataModel->getTask( info.taskId );
            tasks.appendChild( modelTask.toXml( document ) );
//             TaskId parentTask = DATAMODEL->parentItem( modelTask ).task().id();
//             QDomElement task = document.createElement( "task" );
//             task.setAttribute( "taskid", QString::number( info.taskId ) );
//             if ( parentTask != 0 )
//                 task.setAttribute( "parent", QString::number( parentTask ) );

//             QDomText name = document.createTextNode( modelTask.name() );
//             task.appendChild( name );
//             tasks.appendChild( task );
        }
    }
    {   // effort
        // make effort element:
        QDomElement effort = document.createElement( "effort" );
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
                        comment += " / ";
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

//     qDebug() << "WeeklyTimeSheetReport::slotSaveToXml: generated XML:" << endl
//              << document.toString( 4 );
//
    return document.toByteArray( 4 );
}
