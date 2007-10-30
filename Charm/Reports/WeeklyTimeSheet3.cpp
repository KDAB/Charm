#include <QFile>
#include <QTimer>
#include <QSettings>
#include <QDomElement>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>
#include <QtAlgorithms>
#include <QDomDocument>

#include "ViewHelpers.h"
#include "SelectTaskDialog.h"
#include "WeeklyTimeSheet3.h"
#include "CharmReport.h"

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
    if ( settings.contains( MetaKey_TimesheetSubscribedOnly ) ) {
        m_ui->checkBoxSubscribedOnly->setChecked( settings.value( MetaKey_TimesheetSubscribedOnly ).toBool() );
    } else {
        m_ui->checkBoxSubscribedOnly->setChecked( false );
    }
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
    settings.setValue( MetaKey_TimesheetSubscribedOnly,
                       m_ui->checkBoxSubscribedOnly->isChecked() );
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
    int weekNumber = start.weekNumber();
    bool activeOnly = m_ui->checkBoxActiveOnly->isChecked();
    bool subscribedOnly = m_ui->checkBoxSubscribedOnly->isChecked();
    WeeklyTimeSheetReport* report = new WeeklyTimeSheetReport( parent );
    report->setReportProperties( start, end, weekNumber, m_rootTask, activeOnly, subscribedOnly );
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
            m_ui->labelTaskName->setText( tasknameWithParents( item.task() ) );
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
        m_ui->labelTaskName->setText( tasknameWithParents( item.task() ) );
    } else {
        if ( m_rootTask == 0 )
            m_ui->checkBoxSubTasksOnly->setChecked( false );
    }
}

// here begins ... the actual report:

WeeklyTimeSheetReport::WeeklyTimeSheetReport( QWidget* parent )
    : ReportPreviewWindow( parent )
    , m_weekNumber( 0 )
    , m_rootTask( 0 )
    , m_activeTasksOnly( false )
    , m_subscribedOnly( false )
    , m_report( 0 )
{
}

WeeklyTimeSheetReport::~WeeklyTimeSheetReport()
{
}

void WeeklyTimeSheetReport::setReportProperties(
    const QDate& start, const QDate& end,
    int weekNumber, TaskId rootTask,
    bool activeTasksOnly, bool subscribedOnly )
{
    m_start = start;
    m_end = end;
    m_weekNumber = weekNumber;
    m_rootTask = rootTask;
    m_activeTasksOnly = activeTasksOnly;
    m_subscribedOnly = subscribedOnly;

    slotUpdate();
}

// helper functions:
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

// retrieve events that match the settings (active, subscribed, ...):
TimeSheetInfoList filteredTaskWithSubTasks(
    TimeSheetInfoList timeSheetInfo,
    bool activeTasksOnly, bool subscribedOnly )
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

    if ( subscribedOnly ) {
        TimeSheetInfoList subscribed;
        for ( int i = 0; i < timeSheetInfo.size(); ++i ) {
            const TaskTreeItem& item = DATAMODEL->taskTreeItem( timeSheetInfo[i].taskId );
            if ( item.task().subscribed() || timeSheetInfo[i].total() > 0 && timeSheetInfo[i].aggregated ) {
                subscribed << timeSheetInfo[i];
            }
        }
        timeSheetInfo = subscribed;
    }

    return timeSheetInfo;
}

void WeeklyTimeSheetReport::slotUpdate()
{   // this creates the time sheet
    delete m_report; m_report = 0;

    // retrieve matching events:
    EventIdList matchingEvents = eventsThatStartInTimeFrame(
        QDateTime( m_start ), QDateTime( m_end ) );

    const int DaysInWeek = 7;
    SecondsMap secondsMap;

    // for every task, make a vector that includes a number of seconds
    // for every day of the week ( int seconds[7]), and store those in
    // a map by their task id
    Q_FOREACH( EventId id, matchingEvents ) {
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
    m_report = new QTextDocument( this );
    QDomDocument doc = createReportTemplate();
    QDomElement root = doc.documentElement();
    QDomElement body = root.firstChildElement( "body" );

//     QTextCursor cursor( m_report );
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
        // TimeSheetInfoList timeSheetInfo = taskWithSubTasks( m_rootTask, secondsMap );
        TimeSheetInfoList timeSheetInfo = filteredTaskWithSubTasks(
            taskWithSubTasks( m_rootTask, secondsMap ),
            m_activeTasksOnly, m_subscribedOnly );

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
    setDocument( m_report );
}

void  WeeklyTimeSheetReport::slotSaveToXml()
{
    qDebug() << "WeeklyTimeSheet::slotSaveToXml: creating XML time sheet";
    // first, ask for a file name:
    QSettings settings;
    QString path;
    if ( settings.contains( MetaKey_ReportsRecentSavePath ) ) {
        path = settings.value( MetaKey_ReportsRecentSavePath ).toString();
        QDir dir( path );
        if ( !dir.exists() ) path = QString();
    }
    // suggest file name:
    QString suggestedFilename = tr( "WeeklyTimeSheet-%1-%2" )
                                .arg( m_start.year() )
                                .arg( m_weekNumber );
    path += QDir::separator() + suggestedFilename;
    // ask:
    QString filename = QFileDialog::getSaveFileName( this, tr( "Enter File Name" ), path );
    if ( filename.isEmpty() ) return;
    QFileInfo fileinfo( filename );
    path = fileinfo.absolutePath();
    if ( !path.isEmpty() ) {
        settings.setValue( MetaKey_ReportsRecentSavePath, path );
    }
    if ( fileinfo.suffix().isEmpty() ) {
        filename+=".charmreport";
    }

    // now create the report:
    QDomDocument document = createExportTemplate( "weekly-timesheet" );

    // find metadata and report element:
    QDomElement root = document.documentElement();
    QDomElement metadata = root.firstChildElement( "metadata" );
    QDomElement report = root.firstChildElement( "report" );
    Q_ASSERT( !root.isNull() && !metadata.isNull() && !report.isNull() );

    // extend metadata tag: add year, and serial (week) number:
    {
        QDomElement yearElement = document.createElement( "year" );
        metadata.appendChild( yearElement );
        QDomText text = document.createTextNode( QString().setNum( m_start.year() ) );
        yearElement.appendChild( text );
        QDomElement weekElement = document.createElement( "serial-number" );
        weekElement.setAttribute( "semantics", "week-number" );
        metadata.appendChild( weekElement );
        QDomText weektext = document.createTextNode( QString().setNum( m_weekNumber ) );
        weekElement.appendChild( weektext );
    }

    SecondsMap secondsMap;
    TimeSheetInfoList timeSheetInfo = filteredTaskWithSubTasks(
        taskWithSubTasks( m_rootTask, secondsMap ),
        false, m_subscribedOnly ); // here, we don't care about active or not, because we only report on the tasks

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
//             task.setAttribute( "taskid", QString().setNum( info.taskId ) );
//             if ( parentTask != 0 )
//                 task.setAttribute( "parent", QString().setNum( parentTask ) );

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
        EventIdList matchingEvents = eventsThatStartInTimeFrame(
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
                int seconds = events[key].duration() + event.duration();
                QDateTime end( events[key].startDateTime().addSecs( seconds ) );
                events[key].setEndDateTime( end );
                QString comment = events[key].comment();
                if ( ! event.comment().isEmpty() ) {
                    if ( !comment.isEmpty() ) { // make separator
                        comment += " / ";
                    }
                    comment += event.comment();
                    events[key].setComment( comment );
                }
                Q_ASSERT( events[key].duration() == seconds );
            } else {
                // add this event:
                events[key] = event;
                events[key].setId( -events[key].id() ); // "synthetic" :-)
                // move to start at midnight:
                QDateTime start( event.startDateTime() );
                start.setTime( QTime() );
                QDateTime end( start.addSecs( event.duration() ) );
                events[key].setStartDateTime( start );
                events[key].setEndDateTime( end );
                Q_ASSERT( events[key].duration() == event.duration() );
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
        stream << document.toString( 4 );
    } else {
        QMessageBox::critical( this, tr( "Error saving report" ),
                               tr( "Cannot write to selected location." ) );
    }
//     qDebug() << "WeeklyTimeSheetReport::slotSaveToXml: generated XML:" << endl
//              << document.toString( 4 );
}

#include "WeeklyTimeSheet3.moc"
