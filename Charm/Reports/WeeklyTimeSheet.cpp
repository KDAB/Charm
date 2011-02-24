#include <QFile>
#include <QTimer>
#include <QSettings>
#include <QDomElement>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>
#include <QtAlgorithms>
#include <QDomDocument>

#include <Core/CharmExceptions.h>
#include <Core/Dates.h>
#include <Core/XmlSerialization.h>

#include "ViewHelpers.h"
#include "SelectTaskDialog.h"
#include "WeeklyTimeSheet.h"
#include "CharmReport.h"
#include "DateEntrySyncer.h"

#include "ui_WeeklyTimeSheetConfigurationPage.h"

WTSConfigurationPage::WTSConfigurationPage( ReportDialog* parent )
    : ReportConfigurationPage( parent )
    , m_ui( new Ui::WeeklyTimeSheetConfigurationPage )
{
    m_ui->setupUi( this );

    connect( m_ui->pushButtonBack, SIGNAL( clicked() ),
             SIGNAL( back() ) );
    connect( m_ui->pushButtonReport, SIGNAL( clicked() ),
             SLOT( slotOkClicked() ) );
    connect( m_ui->comboBoxWeek, SIGNAL( currentIndexChanged( int ) ),
             SLOT( slotWeekComboItemSelected( int ) ) );
    connect( m_ui->toolButtonSelectTask, SIGNAL( clicked() ),
             SLOT( slotSelectTask() ) );
    connect( m_ui->checkBoxSubTasksOnly, SIGNAL( toggled( bool ) ),
             SLOT( slotCheckboxSubtasksOnlyChecked( bool ) ) );
    m_ui->comboBoxWeek->setCurrentIndex( 1 );
    slotCheckboxSubtasksOnlyChecked( m_ui->checkBoxSubTasksOnly->isChecked() );

    new DateEntrySyncer( m_ui->spinBoxWeek, m_ui->spinBoxYear, m_ui->dateEditDay );

    QTimer::singleShot( 0, this, SLOT( slotDelayedInitialization() ) );
}

WTSConfigurationPage::~WTSConfigurationPage()
{
    delete m_ui; m_ui = 0;
}

void WTSConfigurationPage::slotDelayedInitialization()
{
    slotStandardTimeSpansChanged();
    connect( &Application::instance().timeSpans(),
             SIGNAL( timeSpansChanged() ),
             SLOT( slotStandardTimeSpansChanged() ) );

    // load settings:
    QSettings settings;
    if ( settings.contains( MetaKey_TimesheetActiveOnly ) ) {
        m_ui->checkBoxActiveOnly->setChecked( settings.value( MetaKey_TimesheetActiveOnly ).toBool() );
    } else {
        m_ui->checkBoxActiveOnly->setChecked( true );
    }
}

void WTSConfigurationPage::slotOkClicked()
{
    // save settings:
    QSettings settings;
    settings.setValue( MetaKey_TimesheetActiveOnly,
                       m_ui->checkBoxActiveOnly->isChecked() );
    settings.setValue( MetaKey_TimesheetRootTask,
                       m_rootTask );

    emit accept();
}

QDialog* WTSConfigurationPage::makeReportPreviewDialog( QWidget* parent )
{
    QDate start, end;
    int index = m_ui->comboBoxWeek->currentIndex();
    if ( index == m_weekInfo.size() -1 ) {
        // manual selection
        QDate selectedDate = m_ui->dateEditDay->date();
        start = selectedDate.addDays( - selectedDate.dayOfWeek() + 1 );
        end = start.addDays( 7 );
    } else {
        start = m_weekInfo[index].timespan.first;
        end = m_weekInfo[index].timespan.second;
    }
    bool activeOnly = m_ui->checkBoxActiveOnly->isChecked();
    WeeklyTimeSheetReport* report = new WeeklyTimeSheetReport( parent );
    report->setReportProperties( start, end, m_rootTask, activeOnly );
    return report;
}

QString WTSConfigurationPage::name()
{
    return tr( "Weekly Time Sheet" );
}

void WTSConfigurationPage::showEvent( QShowEvent* )
{
    QSettings settings;

    // we only want to do this once a backend is loaded, and we ignore
    // the saved root task if it does not exist anymore
    if ( settings.contains( MetaKey_TimesheetRootTask ) ) {
        TaskId root = settings.value( MetaKey_TimesheetRootTask ).toInt();
        const TaskTreeItem& item = DATAMODEL->taskTreeItem( root );
        if ( item.isValid() ) {
            m_rootTask = root;
            m_ui->labelTaskName->setText( DATAMODEL->fullTaskName( item.task() ) );
            m_ui->checkBoxSubTasksOnly->setChecked( true );
        }
    }
}

QString WTSConfigurationPage::description()
{
    return tr (
        "Creates a tabular report on all activity within a week. "
        "The report is summarized by task and by day." );
}

void WTSConfigurationPage::slotCheckboxSubtasksOnlyChecked( bool checked )
{
    if ( checked && m_rootTask == 0 ) {
        slotSelectTask();
    }

    if ( ! checked ) {
        m_rootTask = 0;
        m_ui->labelTaskName->setText( tr( "(All Tasks)" ) );
    }
}

void WTSConfigurationPage::slotStandardTimeSpansChanged()
{
    m_weekInfo = Application::instance().timeSpans().last4Weeks();
    NamedTimeSpan custom = {
        tr( "Manual Selection" ),
        Application::instance().timeSpans().thisWeek().timespan
    };
    m_weekInfo << custom;
    m_ui->comboBoxWeek->clear();
    for ( int i = 0; i < m_weekInfo.size(); ++i )
    {
        m_ui->comboBoxWeek->addItem( m_weekInfo[i].name );
    }
    // Set current index to "Last Week" as that's what you'll usually want
    m_ui->comboBoxWeek->setCurrentIndex( 1 );
}

void WTSConfigurationPage::slotWeekComboItemSelected( int index )
{
    // wait for the next update, in this case:
    if ( m_ui->comboBoxWeek->count() == 0 || index == -1 ) return;
    Q_ASSERT( m_ui->comboBoxWeek->count() > index );

    if ( index == m_weekInfo.size() - 1 ) { // manual selection
        m_ui->groupBox->setEnabled( true );
    } else {
        m_ui->dateEditDay->setDate( m_weekInfo[index].timespan.first );
        m_ui->groupBox->setEnabled( false );
    }
}

void WTSConfigurationPage::slotSelectTask()
{
    SelectTaskDialog dialog( this );
    if ( dialog.exec() ) {
        m_rootTask = dialog.selectedTask();
        const TaskTreeItem& item = DATAMODEL->taskTreeItem( m_rootTask );
        m_ui->labelTaskName->setText( DATAMODEL->fullTaskName( item.task() ) );
    } else {
        if ( m_rootTask == 0 )
            m_ui->checkBoxSubTasksOnly->setChecked( false );
    }
}

// here begins ... the actual report:

WeeklyTimeSheetReport::WeeklyTimeSheetReport( QWidget* parent )
    : ReportPreviewWindow( parent )
    , m_weekNumber( 0 )
    , m_yearOfWeek( 0 )
    , m_rootTask( 0 )
    , m_activeTasksOnly( false )
    , m_report( 0 )
{
}

WeeklyTimeSheetReport::~WeeklyTimeSheetReport()
{
}

void WeeklyTimeSheetReport::setReportProperties(
    const QDate& start, const QDate& end,
    TaskId rootTask, bool activeTasksOnly )
{
    m_start = start;
    m_end = end;
    m_rootTask = rootTask;
    m_activeTasksOnly = activeTasksOnly;
    m_weekNumber = start.weekNumber( &m_yearOfWeek );

    slotUpdate();
}

// helper functions:

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
                                           const WeeklyTimeSheetReport::SecondsMap& m_secondsMap,
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
        if ( m_secondsMap.contains( id ) ) {
            myInformation.seconds = m_secondsMap.value(id);
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
        children << taskWithSubTasks( id, m_secondsMap, &myInformation );
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

// retrieve events that match the settings (active, ...):
TimeSheetInfoList filteredTaskWithSubTasks(
    TimeSheetInfoList timeSheetInfo,
    bool activeTasksOnly )
{
    if ( activeTasksOnly ) {
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

    return timeSheetInfo;
}

void WeeklyTimeSheetReport::slotUpdate()
{   // this creates the time sheet
    delete m_report; m_report = 0;

    // retrieve matching events:
    EventIdList matchingEvents = DATAMODEL->eventsThatStartInTimeFrame(
        QDateTime( m_start ), QDateTime( m_end ) );

    const int DaysInWeek = 7;
    m_secondsMap.clear();

    // for every task, make a vector that includes a number of seconds
    // for every day of the week ( int seconds[7]), and store those in
    // a map by their task id
    Q_FOREACH( EventId id, matchingEvents ) {
        const Event& event = DATAMODEL->eventForId( id );
        QVector<int> seconds( DaysInWeek );
        if ( m_secondsMap.contains( event.taskId() ) ) {
            seconds = m_secondsMap.value(event.taskId());
        }
        // what day in the week is the event (normalized to vector indexes):
        int dayOfWeek = event.startDateTime().date().dayOfWeek() - 1;
        Q_ASSERT( dayOfWeek >= 0 && dayOfWeek < DaysInWeek );
        seconds[dayOfWeek] += event.duration();
        // store in minute map:
        m_secondsMap[event.taskId()] = seconds;
    }
    // now the reporting:
    // headline first:
    m_report = new QTextDocument( this );
    QDomDocument doc = createReportTemplate();
    QDomElement root = doc.documentElement();
    QDomElement body = root.firstChildElement( "body" );

//     QTextCursor cursor( m_report );
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
                          .arg( m_weekNumber, 2, 10, QChar('0') )
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
        // TimeSheetInfoList timeSheetInfo = taskWithSubTasks( m_rootTask, m_secondsMap );
        TimeSheetInfoList timeSheetInfo = filteredTaskWithSubTasks(
            taskWithSubTasks( m_rootTask, m_secondsMap ),
            m_activeTasksOnly );

        QDomElement table = doc.createElement( "table" );
        table.setAttribute( "width", "100%" );
        table.setAttribute( "align", "left" );
        table.setAttribute( "cellpadding", "3" );
        table.setAttribute( "cellspacing", "0" );
        body.appendChild( table );

        TimeSheetInfo totalsLine;
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
    setDocument( m_report );
}

void  WeeklyTimeSheetReport::slotSaveToXml()
{
    qDebug() << "WeeklyTimeSheet::slotSaveToXml: creating XML time sheet";
    // first, ask for a file name:
    QString filename = getFileName( "Charm reports (*.charmreport)" );
    if (filename.isEmpty())
        return;

    try {
        // now create the report:
        QDomDocument document = XmlSerialization::createXmlTemplate( "weekly-timesheet" );

        // find metadata and report element:
        QDomElement root = document.documentElement();
        QDomElement metadata = XmlSerialization::metadataElement( document );
        QDomElement report = XmlSerialization::reportElement( document );
        Q_ASSERT( !root.isNull() && !metadata.isNull() && !report.isNull() );

        // extend metadata tag: add year, and serial (week) number:
        {
            QDomElement yearElement = document.createElement( "year" );
            metadata.appendChild( yearElement );
            QDomText text = document.createTextNode( QString::number( m_yearOfWeek ) );
            yearElement.appendChild( text );
            QDomElement weekElement = document.createElement( "serial-number" );
            weekElement.setAttribute( "semantics", "week-number" );
            metadata.appendChild( weekElement );
            QDomText weektext = document.createTextNode( QString::number( m_weekNumber ) );
            weekElement.appendChild( weektext );
        }

        SecondsMap m_secondsMap;
        TimeSheetInfoList timeSheetInfo = filteredTaskWithSubTasks(
            taskWithSubTasks( m_rootTask, m_secondsMap ),
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

            // retrieve it:
            EventIdList matchingEvents = DATAMODEL->eventsThatStartInTimeFrame(
                QDateTime( m_start ), QDateTime( m_end ) );
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
            Q_FOREACH( Event event, events ) {
                effort.appendChild( event.toXml( document ) );
            }
        }

        QFile file( filename );
        if ( file.open( QIODevice::WriteOnly ) ) {
            QTextStream stream( &file );
            document.save( stream, 4 );
        } else {
            QMessageBox::critical( this, tr( "Error saving report" ),
                                   tr( "Cannot write to selected location." ) );
        }
//     qDebug() << "WeeklyTimeSheetReport::slotSaveToXml: generated XML:" << endl
//              << document.toString( 4 );
    } catch ( XmlSerializationException& e ) {
        QMessageBox::critical( this, tr( "Error exporting the report" ), e.what() );
    }
}

QString WeeklyTimeSheetReport::getFileName( const QString& filter )
{
    QSettings settings;
    QString path;
    if ( settings.contains( MetaKey_ReportsRecentSavePath ) ) {
        path = settings.value( MetaKey_ReportsRecentSavePath ).toString();
        QDir dir( path );
        if ( !dir.exists() ) path = QString();
    }
    // suggest file name:
    QString suggestedFilename = tr( "WeeklyTimeSheet-%1-%2" )
                                .arg( m_yearOfWeek )
                                .arg( m_weekNumber, 2, 10, QChar('0') );
    path += QDir::separator() + suggestedFilename;
    // ask:
    QString filename = QFileDialog::getSaveFileName( this, tr( "Enter File Name" ), path, filter );
    if ( filename.isEmpty() )
        return QString();
    QFileInfo fileinfo( filename );
    path = fileinfo.absolutePath();
    if ( !path.isEmpty() ) {
        settings.setValue( MetaKey_ReportsRecentSavePath, path );
    }
    return filename;
}

void WeeklyTimeSheetReport::slotSaveToText()
{
    qDebug() << "WeeklyTimeSheet::slotSaveToText: creating text file with totals";
    // first, ask for a file name:
    const QString filename = getFileName( "Text files (*.txt)" );
    if (filename.isEmpty())
        return;

    QFile file( filename );
    if ( !file.open( QIODevice::WriteOnly ) ) {
        QMessageBox::critical( this, tr( "Error saving report" ),
                               tr( "Cannot write to selected location." ) );
        return;
    }

    QTextStream stream( &file );
    QString content = tr( "Report for %1, Week %2 (%3 to %4)" )
                      .arg( CONFIGURATION.user.name() )
                      .arg( m_weekNumber, 2, 10, QChar('0') )
                      .arg( m_start.toString( Qt::TextDate ) )
                      .arg( m_end.addDays( -1 ).toString( Qt::TextDate ) );
    stream << content << '\n';
    stream << '\n';
    TimeSheetInfoList timeSheetInfo = filteredTaskWithSubTasks(
        taskWithSubTasks( m_rootTask, m_secondsMap ),
        m_activeTasksOnly );

    TimeSheetInfo totalsLine;
    if ( ! timeSheetInfo.isEmpty() ) {
        totalsLine = timeSheetInfo.first();
        if( m_rootTask == 0 ) {
            timeSheetInfo.removeAt( 0 ); // there is always one, because there is always the root item
        }
    }

    for (int i = 0; i < timeSheetInfo.size(); ++i ) {
        stream << timeSheetInfo[i].taskname << "\t" << hoursAndMinutes( timeSheetInfo[i].total() ) << '\n';
    }
    stream << '\n';
    stream << "Week total: " << hoursAndMinutes( totalsLine.total() ) << '\n';
}

#include "WeeklyTimeSheet.moc"
