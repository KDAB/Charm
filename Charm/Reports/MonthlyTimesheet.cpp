#include "MonthlyTimesheet.h"

#include <QFile>
#include <QMessageBox>
#include <QPushButton>

#include <Core/XmlSerialization.h>
#include <Core/Dates.h>

#include "ViewHelpers.h"

#include "CharmCMake.h"

namespace {
    typedef QHash<int, QVector<int> > WeeksByYear;
    static const float SecondsInDay = 60. * 60. * 8. /* eight hour work day */;
}

MonthlyTimeSheetReport::MonthlyTimeSheetReport( QWidget* parent )
    : TimeSheetReport( parent )
    , m_monthNumber( 0 )
    , m_yearOfMonth( 0 )
    , m_weeksInMonth( 0 )
{
}

MonthlyTimeSheetReport::~MonthlyTimeSheetReport()
{
}

void MonthlyTimeSheetReport::setReportProperties(
    const QDate& start, const QDate& end,
    TaskId rootTask, bool activeTasksOnly )
{
    m_monthNumber = start.month();
    m_yearOfMonth = start.year();
    m_weeksInMonth = Charm::weekDifference( start, end.addDays(-1) ) + 1;
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
        TimeSheetInfo::taskWithSubTasks( m_weeksInMonth, rootTask(), secondsMap() ),
        activeTasksOnly() );

    TimeSheetInfo totalsLine( m_weeksInMonth );
    if ( ! timeSheetInfo.isEmpty() ) {
        totalsLine = timeSheetInfo.first();
        if( rootTask() == 0 ) {
            timeSheetInfo.removeAt( 0 ); // there is always one, because there is always the root item
        }
    }

    for (int i = 0; i < timeSheetInfo.size(); ++i ) {
        stream << timeSheetInfo[i].taskname << "\t" << hoursAndMinutes( timeSheetInfo[i].total() ) << '\n';
    }
    stream << '\n';
    stream << "Month total: " << hoursAndMinutes( totalsLine.total() ) << '\n';
    stream.flush();

    return output;
}

QByteArray MonthlyTimeSheetReport::saveToXml()
{
    try {
        // now create the report:
        QDomDocument document = XmlSerialization::createXmlTemplate( "monthly-timesheet" );

        // find metadata and report element:
        QDomElement root = document.documentElement();
        QDomElement metadata = XmlSerialization::metadataElement( document );
        QDomElement charmVersion = document.createElement( "charmversion" );
        QDomText charmVersionString = document.createTextNode( CHARM_VERSION );
        charmVersion.appendChild( charmVersionString );
        metadata.appendChild( charmVersion );
        QDomElement report = XmlSerialization::reportElement( document );
        Q_ASSERT( !root.isNull() && !metadata.isNull() && !report.isNull() );

        // extend metadata tag: add year, and serial (month) number:
        {
            QDomElement yearElement = document.createElement( "year" );
            metadata.appendChild( yearElement );
            QDomText text = document.createTextNode( QString::number( m_yearOfMonth ) );
            yearElement.appendChild( text );
            QDomElement monthElement = document.createElement( "serial-number" );
            monthElement.setAttribute( "semantics", "month-number" );
            metadata.appendChild( monthElement );
            QDomText monthtext = document.createTextNode( QString::number( m_monthNumber ) );
            monthElement.appendChild( monthtext );
        }

        SecondsMap m_secondsMap;
        TimeSheetInfoList timeSheetInfo = TimeSheetInfo::filteredTaskWithSubTasks(
            TimeSheetInfo::taskWithSubTasks( m_weeksInMonth, rootTask(), m_secondsMap ),
            false ); // here, we don't care about active or not, because we only report on the tasks

        // extend report tag: add tasks and effort structure
        {   // tasks
            QDomElement tasks = document.createElement( "tasks" );
            report.appendChild( tasks );
            Q_FOREACH( TimeSheetInfo info, timeSheetInfo ) {
                if ( info.taskId == 0 ) // the root task
                    continue;
                const Task& modelTask = DATAMODEL->getTask( info.taskId );
                tasks.appendChild( modelTask.toXml( document ) );
#if 0
               TaskId parentTask = DATAMODEL->parentItem( modelTask ).task().id();
               QDomElement task = document.createElement( "task" );
               task.setAttribute( "taskid", QString::number( info.taskId ) );
               if ( parentTask != 0 )
                   task.setAttribute( "parent", QString::number( parentTask ) );

               QDomText name = document.createTextNode( modelTask.name() );
               task.appendChild( name );
               tasks.appendChild( task );
#endif
            }
        }
        {   // effort
            // make effort element:
            QDomElement effort = document.createElement( "effort" );
            report.appendChild( effort );

            // retrieve it:
            EventIdList matchingEvents = DATAMODEL->eventsThatStartInTimeFrame( startDate(), endDate() );
            // aggregate (group by task and day):
            typedef QPair<TaskId, QDate> Key;
            QMap< Key, Event> events;
            Q_FOREACH( EventId id, matchingEvents ) {
                const Event& event = DATAMODEL->eventForId( id );
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
            Q_FOREACH( const Event & event, events ) {
                effort.appendChild( event.toXml( document ) );
            }
        }

#if 0
       qDebug() << "MonthlyTimeSheetReport::slotSaveToXml: generated XML:" << endl
                << document.toString( 4 );
#endif

       return document.toByteArray( 4 );
    } catch ( XmlSerializationException& e ) {
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

    secondsMap().clear();
    // for every task, make a vector that includes a number of seconds
    // for every week of a month ( int seconds[m_weeksInMonth]), and store those in
    // a map by their task id
    Q_FOREACH( EventId id, matchingEvents ) {
        const Event& event = DATAMODEL->eventForId( id );
        QVector<int> seconds( m_weeksInMonth );
        if ( secondsMap().contains( event.taskId() ) ) {
            seconds = secondsMap().value(event.taskId());
        }
        // what week of the month is the event (normalized to vector indexes):
        const int weekOfMonth = Charm::weekDifference( startDate(), event.startDateTime().date() );
        seconds[weekOfMonth] += event.duration();
        // store in minute map:
        secondsMap()[event.taskId()] = seconds;
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
        QDomElement paragraph = doc.createElement( "br" );
        body.appendChild( paragraph );
    }
    {
        // now for a table
        // retrieve the information for the report:
        // TimeSheetInfoList timeSheetInfo = taskWithSubTasks( m_rootTask, m_secondsMap );
        TimeSheetInfoList timeSheetInfo = TimeSheetInfo::filteredTaskWithSubTasks(
            TimeSheetInfo::taskWithSubTasks( m_weeksInMonth, rootTask(), secondsMap() ),
            activeTasksOnly() );

        QDomElement table = doc.createElement( "table" );
        table.setAttribute( "width", "100%" );
        table.setAttribute( "align", "left" );
        table.setAttribute( "cellpadding", "3" );
        table.setAttribute( "cellspacing", "0" );
        body.appendChild( table );

        TimeSheetInfo totalsLine( m_weeksInMonth );
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
            for ( int i = 0; i < m_weeksInMonth; ++i )
                addTblHdr( headerRow, tr( "Week" ) );
            addTblHdr( headerRow, tr( "Total" ) );
            addTblHdr( headerRow, tr( "Days" ) );
        }

        {   //Header day row
            QDomElement headerDayRow = doc.createElement( "tr" );
            headerDayRow.setAttribute( "class", "header_row" );
            table.appendChild( headerDayRow );
            addTblHdr( headerDayRow, QString() );
            for ( int i = 0; i < m_weeksInMonth; ++i ) {
                QString label = tr("%1").arg(startDate().addDays( i * 7 ).weekNumber(), 2, 10, QLatin1Char('0') );
                addTblHdr( headerDayRow, label );
            }
            addTblHdr( headerDayRow, QString() );
            addTblHdr( headerDayRow, tr("8 hours") );
        }

        for ( int i = 0; i < timeSheetInfo.size(); ++i )
        {
            QDomElement row = doc.createElement( "tr" );
            if (i % 2)
                row.setAttribute( "class", "alternate_row" );
            table.appendChild( row );

            QDomElement taskCell = addTblCell( row, timeSheetInfo[i].taskname );
            taskCell.setAttribute( "align", "left" );
            taskCell.setAttribute( "style", QString( "text-indent: %1px;" )
                                            .arg( 9 * timeSheetInfo[i].indentation ) );
            for ( int week = 0; week < m_weeksInMonth; ++week )
                addTblCell( row, hoursAndMinutes( timeSheetInfo[i].seconds[week] ) );
            addTblCell( row, hoursAndMinutes( timeSheetInfo[i].total() ) );
            addTblCell( row, QString::number( timeSheetInfo[i].total() / SecondsInDay, 'f', 1) );
        }

        {   // Totals row
            QDomElement totals = doc.createElement( "tr" );
            totals.setAttribute( "class", "header_row" );
            table.appendChild( totals );

            addTblHdr( totals, tr( "Total:" ) );
            for ( int i = 0; i < m_weeksInMonth; ++i )
                addTblHdr( totals, hoursAndMinutes( totalsLine.seconds[i] ) );
            addTblHdr( totals, hoursAndMinutes( totalsLine.total() ) );
            addTblHdr( totals, QString::number( totalsLine.total() / SecondsInDay, 'f', 1) );
        }
    }

    // NOTE: seems like the style sheet has to be set before the html
    // code is pushed into the QTextDocument
    QFile stylesheet( ":/Charm/report_stylesheet.sty" );
    if ( stylesheet.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
        QString style = stylesheet.readAll();
        if ( !style.isEmpty() ) {
            report.setDefaultStyleSheet( style );
        } else {
            qDebug() << "MonthlyTimeSheet::create: default style sheet is empty, too bad";
        }
    } else {
        qDebug() << "MonthlyTimeSheet::create: cannot load report style sheet";
    }

    report.setHtml( doc.toString() );
    setDocument( &report );
    uploadButton()->setVisible(false);
    uploadButton()->setEnabled(false);
}

