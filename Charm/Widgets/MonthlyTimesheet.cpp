/*
  MonthlyTimesheet.cpp

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

#include "MonthlyTimesheet.h"
#include "Reports/MonthlyTimesheetXmlWriter.h"

#include <QFile>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QUrl>

#include <Core/Dates.h>

#include "ViewHelpers.h"

#include "CharmCMake.h"

namespace {
    typedef QHash<int, QVector<int> > WeeksByYear;
    static float SecondsInDay = 60. * 60. * 8. /* eight hour work day */;
}

MonthlyTimeSheetReport::MonthlyTimeSheetReport( QWidget* parent )
    : TimeSheetReport( parent )
    , m_numberOfWeeks( 0 )
    , m_monthNumber( 0 )
    , m_yearOfMonth( 0 )
{
    QSettings settings;
    settings.beginGroup("users");
    m_weeklyhours = settings.value("weeklyhours").toString().trimmed();
    settings.endGroup();
    m_dailyhours = m_weeklyhours.toInt() / 5;
    if (m_dailyhours>0 &&m_dailyhours<=8)
    {
        SecondsInDay = 60. * 60. * m_dailyhours;
    }
    else
        m_dailyhours = 8;

    connect( this, SIGNAL(anchorClicked(QUrl)), SLOT(slotLinkClicked(QUrl)) );
}

MonthlyTimeSheetReport::~MonthlyTimeSheetReport()
{
}

void MonthlyTimeSheetReport::setReportProperties(
    const QDate& start, const QDate& end,
    TaskId rootTask, bool activeTasksOnly )
{
    m_numberOfWeeks = Charm::weekDifference( start, end.addDays(-1) ) + 1;
    m_monthNumber = start.month();
    m_yearOfMonth = start.year();
    TimeSheetReport::setReportProperties(start, end, rootTask, activeTasksOnly);
}

QString MonthlyTimeSheetReport::suggestedFileName() const
{
    return tr( "MonthlyTimeSheet-%1-%2" ).arg( m_yearOfMonth ).arg( m_monthNumber, 2, 10, QChar('0') );
}

QByteArray MonthlyTimeSheetReport::saveToText()
{
    QByteArray output;
    QTextStream stream( &output );
    QString content = tr( "Report for %1, %2 %3 (%4 to %5)" )
                      .arg( CONFIGURATION.user.name() )
                      .arg( QDate::longMonthName( m_monthNumber ) )
                      .arg( startDate().year() )
                      .arg( startDate().toString( Qt::TextDate ) )
                      .arg( endDate().addDays( -1 ).toString( Qt::TextDate ) );
    stream << content << '\n';
    stream << '\n';
    TimeSheetInfoList timeSheetInfo = TimeSheetInfo::filteredTaskWithSubTasks(
        TimeSheetInfo::taskWithSubTasks( DATAMODEL, m_numberOfWeeks, rootTask(), secondsMap() ),
        activeTasksOnly() );

    TimeSheetInfo totalsLine( m_numberOfWeeks );
    if ( ! timeSheetInfo.isEmpty() ) {
        totalsLine = timeSheetInfo.first();
        if( rootTask() == 0 ) {
            timeSheetInfo.removeAt( 0 ); // there is always one, because there is always the root item
        }
    }

    for ( int i = 0; i < timeSheetInfo.size(); ++i ) {
        stream << timeSheetInfo[i].formattedTaskIdAndName( CONFIGURATION.taskPaddingLength ) << "\t" << hoursAndMinutes( timeSheetInfo[i].total() ) << '\n';
    }
    stream << '\n';
    stream << "Month total: " << hoursAndMinutes( totalsLine.total() ) << '\n';
    stream.flush();

    return output;
}

QByteArray MonthlyTimeSheetReport::saveToXml()
{
    try {
        MonthlyTimesheetXmlWriter timesheet;
        timesheet.setDataModel( DATAMODEL );
        timesheet.setMonthNumber( m_monthNumber );
        timesheet.setYearOfMonth( m_yearOfMonth );
        timesheet.setNumberOfWeeks( m_numberOfWeeks );
        timesheet.setRootTask( rootTask() );
        const EventIdList matchingEventIds = DATAMODEL->eventsThatStartInTimeFrame( startDate(), endDate() );
        EventList events;
        events.reserve( matchingEventIds.size() );
        Q_FOREACH ( const EventId& eventId, matchingEventIds ) {
            events.append( DATAMODEL->eventForId( eventId ) );
        }
        timesheet.setEvents( events );
        return timesheet.saveToXml();
    } catch ( const XmlSerializationException& e ) {
        QMessageBox::critical( this, tr( "Error exporting the report" ), e.what() );
    }

    return QByteArray();
}

static QDomElement addTblHdr( QDomElement &toRow, const QString &text )
{
    QDomElement header = toRow.ownerDocument().createElement( "th" );
    QDomText textNode = toRow.ownerDocument().createTextNode( text );
    header.appendChild( textNode );
    toRow.appendChild( header );
    return header;
}

static QDomElement addTblCell( QDomElement &toRow, const QString &text )
{
    QDomElement cell = toRow.ownerDocument().createElement( "td" );
    cell.setAttribute( "align", "center" );
    QDomText textNode = toRow.ownerDocument().createTextNode( text );
    cell.appendChild( textNode );
    toRow.appendChild( cell );
    return cell;
}

void MonthlyTimeSheetReport::update()
{
    // this creates the time sheet
    // retrieve matching events:
    const EventIdList matchingEvents = DATAMODEL->eventsThatStartInTimeFrame( startDate(), endDate() );

    m_secondsMap.clear();

    // for every task, make a vector that includes a number of seconds
    // for every week of a month ( int seconds[m_numberOfWeeks]), and store those in
    // a map by their task id
    Q_FOREACH( EventId id, matchingEvents ) {
        const Event& event = DATAMODEL->eventForId( id );
        QVector<int> seconds( m_numberOfWeeks );
        if ( m_secondsMap.contains( event.taskId() ) ) {
            seconds = m_secondsMap.value(event.taskId());
        }
        // what week of the month is the event (normalized to vector indexes):
        const int weekOfMonth = Charm::weekDifference( startDate(), event.startDateTime().date() );
        seconds[weekOfMonth] += event.duration();
        // store in minute map:
        m_secondsMap[event.taskId()] = seconds;
    }
    // now the reporting:
    // headline first:
    QTextDocument report;
    QDomDocument doc = createReportTemplate();
    QDomElement root = doc.documentElement();
    QDomElement body = root.firstChildElement( "body" );

//     QTextCursor cursor( m_report );
    // create the caption:
    {
        QDomElement headline = doc.createElement( "h1" );
        QDomText text = doc.createTextNode( tr( "Monthly Time Sheet" ) );
        headline.appendChild( text );
        body.appendChild( headline );
    }
    {
        QDomElement headline = doc.createElement( "h3" );
        QString content = tr( "Report for %1, %2 %3 (%4 to %5)" )
                          .arg( CONFIGURATION.user.name() )
                          .arg( QDate::longMonthName( m_monthNumber ) )
                          .arg( startDate().year() )
                          .arg( startDate().toString( Qt::TextDate ) )
                          .arg( endDate().addDays( -1 ).toString( Qt::TextDate ) );
        QDomText text = doc.createTextNode( content );
        headline.appendChild( text );
        body.appendChild( headline );
        QDomElement previousLink = doc.createElement( "a" );
        previousLink.setAttribute( "href" , "Previous" );
        QDomText previousLinkText = doc.createTextNode( tr( "<Previous Month>" ) );
        previousLink.appendChild( previousLinkText );
        body.appendChild( previousLink );
        QDomElement nextLink = doc.createElement( "a" );
        nextLink.setAttribute( "href" , "Next" );
        QDomText nextLinkText = doc.createTextNode( tr( "<Next Month>" ) );
        nextLink.appendChild( nextLinkText );
        body.appendChild( nextLink );
        QDomElement paragraph = doc.createElement( "br" );
        body.appendChild( paragraph );
    }
    {
        // now for a table
        // retrieve the information for the report:
        // TimeSheetInfoList timeSheetInfo = taskWithSubTasks( m_rootTask, m_secondsMap );
        TimeSheetInfoList timeSheetInfo = TimeSheetInfo::filteredTaskWithSubTasks(
            TimeSheetInfo::taskWithSubTasks( DATAMODEL, m_numberOfWeeks, rootTask(), secondsMap() ),
            activeTasksOnly() );

        QDomElement table = doc.createElement( "table" );
        table.setAttribute( "width", "100%" );
        table.setAttribute( "align", "left" );
        table.setAttribute( "cellpadding", "3" );
        table.setAttribute( "cellspacing", "0" );
        body.appendChild( table );

        TimeSheetInfo totalsLine( m_numberOfWeeks );
        if ( ! timeSheetInfo.isEmpty() ) {
            totalsLine = timeSheetInfo.first();
            if( rootTask() == 0 ) {
                timeSheetInfo.removeAt( 0 ); // there is always one, because there is always the root item
            }
        }

        {   //Header Row
            QDomElement headerRow = doc.createElement( "tr" );
            headerRow.setAttribute( "class", "header_row" );
            table.appendChild( headerRow );
            addTblHdr( headerRow, tr( "Task" ) );
            for ( int i = 0; i < m_numberOfWeeks; ++i )
                addTblHdr( headerRow, tr( "Week" ) );
            addTblHdr( headerRow, tr( "Total" ) );
            addTblHdr( headerRow, tr( "Days" ) );
        }

        {   //Header day row
            QDomElement headerDayRow = doc.createElement( "tr" );
            headerDayRow.setAttribute( "class", "header_row" );
            table.appendChild( headerDayRow );
            addTblHdr( headerDayRow, QString() );
            for ( int i = 0; i < m_numberOfWeeks; ++i ) {
                QString label = tr("%1").arg(startDate().addDays( i * 7 ).weekNumber(), 2, 10, QLatin1Char('0') );
                addTblHdr( headerDayRow, label );
            }
            addTblHdr( headerDayRow, QString() );
            addTblHdr( headerDayRow, QString::number(m_dailyhours) + tr(" hours") );
        }

        for ( int i = 0; i < timeSheetInfo.size(); ++i )
        {
            QDomElement row = doc.createElement( "tr" );
            if (i % 2)
                row.setAttribute( "class", "alternate_row" );
            table.appendChild( row );

            QDomElement taskCell = addTblCell( row, timeSheetInfo[i].formattedTaskIdAndName( CONFIGURATION.taskPaddingLength ) );
            taskCell.setAttribute( "align", "left" );
            taskCell.setAttribute( "style", QString( "text-indent: %1px;" )
                                            .arg( 9 * timeSheetInfo[i].indentation ) );
            for ( int week = 0; week < m_numberOfWeeks; ++week )
                addTblCell( row, hoursAndMinutes( timeSheetInfo[i].seconds[week] ) );
            addTblCell( row, hoursAndMinutes( timeSheetInfo[i].total() ) );
            addTblCell( row, QString::number( timeSheetInfo[i].total() / SecondsInDay, 'f', 1) );
        }

        {   // Totals row
            QDomElement totals = doc.createElement( "tr" );
            totals.setAttribute( "class", "header_row" );
            table.appendChild( totals );

            addTblHdr( totals, tr( "Total:" ) );
            for ( int i = 0; i < m_numberOfWeeks; ++i )
                addTblHdr( totals, hoursAndMinutes( totalsLine.seconds[i] ) );
            addTblHdr( totals, hoursAndMinutes( totalsLine.total() ) );
            addTblHdr( totals, QString::number( totalsLine.total() / SecondsInDay, 'f', 1) );
        }
    }

    // NOTE: seems like the style sheet has to be set before the html
    // code is pushed into the QTextDocument
    report.setDefaultStyleSheet(Charm::reportStylesheet(palette()));

    report.setHtml( doc.toString() );
    setDocument( &report );
    uploadButton()->setVisible(false);
    uploadButton()->setEnabled(false);
}

void MonthlyTimeSheetReport::slotLinkClicked( const QUrl& which )
{
    QDate start = which.toString() == "Previous" ? startDate().addMonths( -1 ) : startDate().addMonths( 1 );
    QDate end = which.toString() == "Previous" ? endDate().addMonths( -1 ) : endDate().addMonths( 1 );
    setReportProperties( start, end, rootTask(), activeTasksOnly() );
}
