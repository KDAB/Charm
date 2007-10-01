#include <QMap>
#include <QFile>
#include <QTimer>
#include <QVector>
#include <QDomElement>
#include <QtAlgorithms>
#include <QDomDocument>

#include "Charm.h"
#include "ReportDialog.h"
#include "SelectTaskDialog.h"
#include "WeeklyTimeSheet.h"
#include "ReportPreviewWindow.h"
#include "ParagraphFormatCollection.h"

WeeklyTimeSheet::WeeklyTimeSheet( QObject* parent )
    : CharmReport( parent )
    , m_rootTask( 0 )
    , m_configurationPage( 0 )
    , m_report( 0 )
{
}

QString WeeklyTimeSheet::name()
{
    return QObject::tr( "Weekly Time Sheet" );
}

QString WeeklyTimeSheet::description()
{
    return QObject::tr
        ( "Creates a tabular report on all activity within a week. "
          "The report is summarized by task and by day." );
}

bool WeeklyTimeSheet::prepare()
{
    int index = m_ui.comboBoxWeek->currentIndex();
    if ( index == m_weekInfo.size() -1 ) {
        // manual selection
        QDate selectedDate = m_ui.dateEditDay->date();
        m_start = selectedDate.addDays( - selectedDate.dayOfWeek() + 1 );
        m_end = m_start.addDays( 7 );
    } else {
        m_start = m_weekInfo[index].timespan.first;
        m_end = m_weekInfo[index].timespan.second;
    }

    m_matchingEvents = eventsThatStartInTimeFrame( QDateTime( m_start ),
                                                   QDateTime( m_end ) );

    // the week number
    m_weekNumber = m_start.weekNumber();

    qDebug() << "WeeklyTimeSheet::prepare: start:"  << m_start
             << ", end:" << m_end << ", week number:"
             << m_weekNumber;

    return true;
}

typedef QMap< TaskId, QVector<int> > SecondsMap;

class TimeSheetInfo {
public:
    TimeSheetInfo()
      : indentation( 0 )
      , seconds( 7 )
      , taskId( 0 )
      , aggregated( false )
    {
        seconds.fill( 0 );
    }

    int total() const
    {
        int value = 0;
        for ( int i = 0; i < seconds.size(); ++i )
            value += seconds[i];
        return value;
    }

    void dump()
    {
        qDebug() << "TimeSheetInfo: (" << indentation << ")" << taskname << ":" << seconds << "-" << total() << "total";
    }

    // the level of indentation, >0 means the numbers are aggregated for the subtasks:
    int indentation;
    // task name string:
    QString taskname;
    // seconds for every weekday:
    QVector<int> seconds;
    // the task id
    TaskId taskId;
    // values are aggregated for subtasks
    bool aggregated;
};

typedef QList<TimeSheetInfo> TimeSheetInfoList;

// make the list, aggregate the seconds in the subtask:
static TimeSheetInfoList taskWithSubTasks( TaskId id,
                                    const SecondsMap& secondsMap,
                                    TimeSheetInfo* addTo = 0 )
{
    TimeSheetInfoList result;
    TimeSheetInfoList children;

    TimeSheetInfo myInformation;
    const TaskTreeItem& item = DATAMODEL->taskTreeItem( id );
    // real task or virtual root item
    Q_ASSERT( item.task().isValid() || id == 0 );

    if ( id != 0 ) {
        // add totals for task itself:
        if ( secondsMap.contains( id ) ) {
            myInformation.seconds = secondsMap[id];
        }
        // add name:
        QTextStream stream( &myInformation.taskname );
        stream << QString("%1" ).arg(
            item.task().id(),
            CONFIGURATION.taskPaddingLength,
            10, QChar( '0' ) )
               << ": " << item.task().name();

        if ( addTo != 0 ) {
            myInformation.indentation = addTo->indentation + 1;
        }
        myInformation.taskId = id;
    } else {
        myInformation.indentation = -1;
    }

    TaskIdList childIds = item.childIds();
    // sort by task id
    qSort( childIds );
    // recursively add those to myself:
    Q_FOREACH( TaskId id, childIds ) {
        children << taskWithSubTasks( id, secondsMap, &myInformation );
    }

    // add to parent:
    if ( addTo != 0 ) {
        for ( int i = 0; i < 7; ++i )
        {
            addTo->seconds[i] += myInformation.seconds[i];
        }
        addTo->aggregated = true;
    }

    result << myInformation << children;

    return result;
}

bool WeeklyTimeSheet::create()
{
    const int DaysInWeek = 7;
    SecondsMap secondsMap;
    // for every task, make a vector that includes a number of seconds
    // for every day of the week ( int seconds[7]), and store those in
    // a map by their task id
    Q_FOREACH( EventId id, m_matchingEvents ) {
        const Event& event = DATAMODEL->eventForId( id );
        QVector<int> seconds( DaysInWeek );
        if ( secondsMap.contains( event.taskId() ) ) {
            seconds = secondsMap[event.taskId()];
        }
        // what day in the week is the event (normalized to vector indexes):
        int dayOfWeek = event.startDateTime().date().dayOfWeek() - 1;
        Q_ASSERT( dayOfWeek >= 0 && dayOfWeek < DaysInWeek );
        seconds[dayOfWeek] += event.duration();
        // store in minute map:
        secondsMap[event.taskId()] = seconds;
    }
    // now the reporting:
    // headline first:
    m_report = new QTextDocument;
    QDomDocument doc = createReportTemplate();
    QDomElement root = doc.documentElement();
    QDomElement body = root.firstChildElement( "body" );

    QTextCursor cursor( m_report );
    TimeSheetInfo totalsLine;
    // create the caption:
    {
        QDomElement headline = doc.createElement( "h1" );
        QDomText text = doc.createTextNode( tr( "Weekly Time Sheet" ) );
        headline.appendChild( text );
        body.appendChild( headline );
    }
    {
        QDomElement headline = doc.createElement( "h3" );
        QString content = tr( "Report for %1, Week %2 (%3 to %4)" )
                          .arg( CONFIGURATION.user.name() )
                          .arg( m_weekNumber )
                          .arg( m_start.toString( Qt::TextDate ) )
                          .arg( m_end.addDays( -1 ).toString( Qt::TextDate ) );
        QDomText text = doc.createTextNode( content );
        headline.appendChild( text );
        body.appendChild( headline );
        QDomElement paragraph = doc.createElement( "br" );
        body.appendChild( paragraph );
    }
    {
        // now for a table
        // retrieve the information for the report:
        TimeSheetInfoList timeSheetInfo = taskWithSubTasks( m_rootTask, secondsMap );

        if ( m_ui.checkBoxActiveOnly->isChecked() ) {
            TimeSheetInfoList nonZero;
            // FIXME use algorithm (I just hate to lug the fat book around)
            for ( int i = 0; i < timeSheetInfo.size(); ++i )
            {
                if ( timeSheetInfo[i].total() > 0 ) {
                    nonZero << timeSheetInfo[i];
                }
            }
            timeSheetInfo = nonZero;
        }

        if ( m_ui.checkBoxSubscribedOnly->isChecked() ) {
            TimeSheetInfoList subscribed;
            for ( int i = 0; i < timeSheetInfo.size(); ++i ) {
                const TaskTreeItem& item = DATAMODEL->taskTreeItem( timeSheetInfo[i].taskId );
                if ( item.task().subscribed() || timeSheetInfo[i].total() > 0 && timeSheetInfo[i].aggregated ) {
                    subscribed << timeSheetInfo[i];
                }
            }
            timeSheetInfo = subscribed;
        }

        QDomElement table = doc.createElement( "table" );
        table.setAttribute( "width", "100%" );
        table.setAttribute( "align", "left" );
        table.setAttribute( "cellpadding", "3" );
        table.setAttribute( "cellspacing", "0" );
        body.appendChild( table );

        if ( ! timeSheetInfo.isEmpty() ) {
            totalsLine = timeSheetInfo.first();
            if( m_rootTask == 0 ) {
                timeSheetInfo.removeAt( 0 ); // there is always one, because there is always the root item
            }
        }

        QDomElement headerRow = doc.createElement( "tr" );
        headerRow.setAttribute( "class", "header_row" );
        table.appendChild( headerRow );
        QDomElement headerDayRow = doc.createElement( "tr" );
        headerDayRow.setAttribute( "class", "header_row" );
        table.appendChild( headerDayRow );

        const QString Headlines[NumberOfColumns] = {
            tr( "Task" ),
            QDate::shortDayName( 1 ),
            QDate::shortDayName( 2 ),
            QDate::shortDayName( 3 ),
            QDate::shortDayName( 4 ),
            QDate::shortDayName( 5 ),
            QDate::shortDayName( 6 ),
            QDate::shortDayName( 7 ),
            tr( "Total" )
        };
        const QString DayHeadlines[NumberOfColumns] = {
            QString(),
            tr( "%1" ).arg( m_start.day(), 2, 10, QLatin1Char('0') ),
            tr( "%1" ).arg( m_start.addDays( 1 ).day(), 2, 10, QLatin1Char('0') ),
            tr( "%1" ).arg( m_start.addDays( 2 ).day(), 2, 10, QLatin1Char('0') ),
            tr( "%1" ).arg( m_start.addDays( 3 ).day(), 2, 10, QLatin1Char('0') ),
            tr( "%1" ).arg( m_start.addDays( 4 ).day(), 2, 10, QLatin1Char('0') ),
            tr( "%1" ).arg( m_start.addDays( 5 ).day(), 2, 10, QLatin1Char('0') ),
            tr( "%1" ).arg( m_start.addDays( 6 ).day(), 2, 10, QLatin1Char('0') ),
            QString()
        };

        for ( int i = 0; i < NumberOfColumns; ++i )
        {
            QDomElement header = doc.createElement( "th" );
            QDomText text = doc.createTextNode( Headlines[i] );
            header.appendChild( text );
            headerRow.appendChild( header );
            QDomElement dayHeader = doc.createElement( "th" );
            QDomText dayText = doc.createTextNode( DayHeadlines[i] );
            dayHeader.appendChild( dayText );
            headerDayRow.appendChild( dayHeader );
        }

        for ( int i = 0; i < timeSheetInfo.size(); ++i )
        {
            QDomElement row = doc.createElement( "tr" );
            table.appendChild( row );

            QString texts[NumberOfColumns];
            texts[Column_Task] = timeSheetInfo[i].taskname;
            texts[Column_Monday] = hoursAndMinutes( timeSheetInfo[i].seconds[0] );
            texts[Column_Tuesday] = hoursAndMinutes( timeSheetInfo[i].seconds[1] );
            texts[Column_Wednesday] = hoursAndMinutes( timeSheetInfo[i].seconds[2] );
            texts[Column_Thursday] = hoursAndMinutes( timeSheetInfo[i].seconds[3] );
            texts[Column_Friday] = hoursAndMinutes( timeSheetInfo[i].seconds[4] );
            texts[Column_Saturday] = hoursAndMinutes( timeSheetInfo[i].seconds[5] );
            texts[Column_Sunday] = hoursAndMinutes( timeSheetInfo[i].seconds[6] );
            texts[Column_Total] = hoursAndMinutes( timeSheetInfo[i].total() );

            for ( int column = 0; column < NumberOfColumns; ++column )
            {
                QDomElement cell = doc.createElement( "td" );
                cell.setAttribute( "align", column == Column_Task ? "left" : "center" );

                if ( column == Column_Task ) {
                    QString style = QString( "text-indent: %1px;" )
                            .arg( 9 * timeSheetInfo[i].indentation );
                    cell.setAttribute( "style", style );
                }


                QDomText text = doc.createTextNode( texts[column] );
                cell.appendChild( text );
                row.appendChild( cell );
            }
        }
        // put the totals:
        QString TotalsTexts[NumberOfColumns] = {
            tr( "Total:" ),
            hoursAndMinutes( totalsLine.seconds[0] ),
            hoursAndMinutes( totalsLine.seconds[1] ),
            hoursAndMinutes( totalsLine.seconds[2] ),
            hoursAndMinutes( totalsLine.seconds[3] ),
            hoursAndMinutes( totalsLine.seconds[4] ),
            hoursAndMinutes( totalsLine.seconds[5] ),
            hoursAndMinutes( totalsLine.seconds[6] ),
            hoursAndMinutes( totalsLine.total() )
        };
        QDomElement totals = doc.createElement( "tr" );
        totals.setAttribute( "class", "header_row" );
        table.appendChild( totals );
        for ( int i = 0; i < NumberOfColumns; ++i )
        {
            QDomElement cell = doc.createElement( "th" );
            QDomText text = doc.createTextNode( TotalsTexts[i] );
            cell.appendChild( text );
            totals.appendChild( cell );
        }
    }

    // NOTE: seems like the style sheet has to be set before the html
    // code is pushed into the QTextDocument
    QFile stylesheet( ":/Charm/report_stylesheet.sty" );
    if ( stylesheet.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
        QString style = stylesheet.readAll();
        if ( !style.isEmpty() ) {
            m_report->setDefaultStyleSheet( style );
        } else {
            qDebug() << "WeeklyTimeSheet::create: default style sheet is empty, too bad";
        }
    } else {
        qDebug() << "WeeklyTimeSheet::create: cannot load report style sheet";
    }
    m_report->setHtml( doc.toString() );
    return true;
}

QTextDocument* WeeklyTimeSheet::report()
{
    return m_report;
}

QWidget* WeeklyTimeSheet::configurationPage( ReportDialog* dialog )
{
    if ( m_configurationPage == 0 ) {
        m_configurationPage = new QWidget;
        m_ui.setupUi( m_configurationPage );
        connect( m_ui.pushButtonBack, SIGNAL( clicked() ),
                 dialog, SLOT( back() ) );
        connect( m_ui.pushButtonReport, SIGNAL( clicked() ),
                 SLOT( makeReportPreviewWindow() ) );
        connect( m_ui.comboBoxWeek, SIGNAL( currentIndexChanged( int ) ),
                 SLOT( slotWeekComboItemSelected( int ) ) );
        connect( m_ui.toolButtonSelectTask, SIGNAL( clicked() ),
                 SLOT( slotSelectTask() ) );
        connect( m_ui.checkBoxSubTasksOnly, SIGNAL( toggled( bool ) ),
                 SLOT( slotCheckboxSubtasksOnlyChecked( bool ) ) );

        m_ui.comboBoxWeek->setCurrentIndex( 1 );
        m_ui.checkBoxSubTasksOnly->setChecked( false );
        m_ui.checkBoxSubscribedOnly->setChecked( true );
        slotCheckboxSubtasksOnlyChecked( m_ui.checkBoxSubTasksOnly->isChecked() );

        QTimer::singleShot( 0, this, SLOT( slotDelayedInitialization() ) );
    }
    return m_configurationPage;
}

void WeeklyTimeSheet::slotDelayedInitialization()
{
    slotStandardTimeSpansChanged();
    connect( &Application::instance().timeSpans(),
             SIGNAL( timeSpansChanged() ),
             SLOT( slotStandardTimeSpansChanged() ) );
}

void WeeklyTimeSheet::slotStandardTimeSpansChanged()
{
    m_weekInfo = Application::instance().timeSpans().last4Weeks();
    NamedTimeSpan custom = {
        tr( "Manual Selection" ),
        Application::instance().timeSpans().thisWeek().timespan
    };
    m_weekInfo << custom;

    m_ui.comboBoxWeek->clear();
    for ( int i = 0; i < m_weekInfo.size(); ++i )
    {
        m_ui.comboBoxWeek->addItem( m_weekInfo[i].name );
    }
}

void WeeklyTimeSheet::slotWeekComboItemSelected( int index )
{
    // wait for the next update, in this case:
    if ( m_ui.comboBoxWeek->count() == 0 || index == -1 ) return;
    Q_ASSERT( m_ui.comboBoxWeek->count() > index );

    if ( index == m_weekInfo.size() - 1 ) { // manual selection
        m_ui.groupBox->setEnabled( true );
    } else {
        m_ui.dateEditDay->setDate( m_weekInfo[index].timespan.first );
        m_ui.groupBox->setEnabled( false );
    }
}

void WeeklyTimeSheet::slotSelectTask()
{
    SelectTaskDialog dialog( m_configurationPage );
    if ( dialog.exec() ) {
        m_rootTask = dialog.selectedTask();
        const TaskTreeItem& item = DATAMODEL->taskTreeItem( m_rootTask );
        m_ui.labelTaskName->setText( tasknameWithParents( item.task() ) );
    } else {
        if ( m_rootTask == 0 )
            m_ui.checkBoxSubTasksOnly->setChecked( false );
    }
}

void WeeklyTimeSheet::slotCheckboxSubtasksOnlyChecked( bool checked )
{
    if ( checked && m_rootTask == 0 ) {
        slotSelectTask();
    }

    if ( ! checked ) {
        m_rootTask = 0;
        m_ui.labelTaskName->setText( tr( "(All Tasks)" ) );
    }
}

QDomDocument WeeklyTimeSheet::createReportTemplate()
{
    // create XHTML v1.0 structure:
    QDomDocument doc( "html" );
    // FIXME this is only a rudimentary subset of a valid xhtml 1 document

    // html element
    QDomElement html = doc.createElement( "html" );
    html.setAttribute( "xmlns", "http://www.w3.org/1999/xhtml" );
    doc.appendChild( html );

    // head and body, children of html
    QDomElement head = doc.createElement( "head" );
    html.appendChild( head );
    QDomElement body = doc.createElement( "body" );
    html.appendChild( body );

    return doc;
}


#include "WeeklyTimeSheet.moc"

