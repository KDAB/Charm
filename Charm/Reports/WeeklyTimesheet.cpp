#include <QFile>
#include <QTimer>
#include <QSettings>
#include <QCalendarWidget>
#include <QDomElement>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>
#include <QtAlgorithms>
#include <QDomDocument>
#include <QPushButton>

#include <Core/CharmExceptions.h>
#include <Core/Dates.h>
#include <Core/XmlSerialization.h>

#include "ViewHelpers.h"
#include "SelectTaskDialog.h"
#include "WeeklyTimesheet.h"
#include "CharmReport.h"
#include "DateEntrySyncer.h"
#include "HttpClient/UploadTimesheetJob.h"

#include "CharmCMake.h"

#include "CharmCMake.h"

#include "ui_WeeklyTimesheetConfigurationDialog.h"

const char * SETTING_GRP_TIMESHEETS = "timesheets";
const char * SETTING_VAL_FIRSTYEAR = "firstYear";
const char * SETTING_VAL_FIRSTWEEK = "firstWeek";
const int MAX_WEEK = 53;
const int MIN_YEAR = 1990;

int weeksInYear(int year)
{
    QDate d(year, 1, 1);
    d = d.addDays(d.daysInYear() - 1);
    return d.weekNumber() == 1 ? 52 : d.weekNumber();
}

void addUploadedTimesheet(int year, int week)
{
    Q_ASSERT(year >= MIN_YEAR && week > 0 && week <= MAX_WEEK);
    QSettings settings;
    settings.beginGroup(SETTING_GRP_TIMESHEETS);
    QString yearStr = QString::number(year);
    QString weekStr = QString::number(week);
    QStringList existingSheets = settings.value(yearStr).toStringList();
    if (!existingSheets.contains(weekStr))
        settings.setValue(yearStr, existingSheets << weekStr);
    if (settings.value(SETTING_VAL_FIRSTYEAR, QString()).toString().isEmpty())
        settings.setValue(SETTING_VAL_FIRSTYEAR, yearStr);
    if (settings.value(SETTING_VAL_FIRSTWEEK, QString()).toString().isEmpty())
        settings.setValue(SETTING_VAL_FIRSTWEEK, weekStr);
}

WeeksByYear missingTimeSheets()
{
    WeeksByYear missing;
    QSettings settings;
    settings.beginGroup(SETTING_GRP_TIMESHEETS);
    int year = QDateTime::currentDateTime().date().year();
    int week = QDateTime::currentDateTime().date().weekNumber();
    int firstYear = settings.value(SETTING_VAL_FIRSTYEAR, year).value<int>();
    int firstWeek = settings.value(SETTING_VAL_FIRSTWEEK, week).value<int>();
    for(int iYear = firstYear; iYear <= year; ++iYear)
    {
        QStringList uploaded = settings.value(QString::number(iYear)).toStringList();
        int firstWeekOfYear = iYear == firstYear ? firstWeek : 1;
        int lastWeekOfYear = iYear == year ? week - 1 : weeksInYear(iYear);
        for(int iWeek = firstWeekOfYear; iWeek <= lastWeekOfYear; ++iWeek)
        {
            if (!uploaded.contains(QString::number(iWeek)))
            {
                Q_ASSERT(iYear >= MIN_YEAR && iWeek > 0 && iWeek <= MAX_WEEK);
                missing[iYear].append(iWeek);
            }
        }
    }
    return missing;
}

WeeklyTimesheetConfigurationDialog::WeeklyTimesheetConfigurationDialog( QWidget* parent )
    : ReportConfigurationDialog( parent )
    , m_ui( new Ui::WeeklyTimesheetConfigurationDialog )
{
    setWindowTitle( tr( "Weekly Timesheet" ) );

    m_ui->setupUi( this );
    m_ui->dateEditDay->calendarWidget()->setFirstDayOfWeek( Qt::Monday );
    connect( m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()) );
    connect( m_ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()) );

    connect( m_ui->comboBoxWeek, SIGNAL( currentIndexChanged( int ) ),
             SLOT( slotWeekComboItemSelected( int ) ) );
    connect( m_ui->toolButtonSelectTask, SIGNAL( clicked() ),
             SLOT( slotSelectTask() ) );
    connect( m_ui->checkBoxSubTasksOnly, SIGNAL( toggled( bool ) ),
             SLOT( slotCheckboxSubtasksOnlyChecked( bool ) ) );
    m_ui->comboBoxWeek->setCurrentIndex( 1 );
    slotCheckboxSubtasksOnlyChecked( m_ui->checkBoxSubTasksOnly->isChecked() );
    new DateEntrySyncer( m_ui->spinBoxWeek, m_ui->spinBoxYear, m_ui->dateEditDay, 1, this );

    slotStandardTimeSpansChanged();
    connect( Application::instance().dateChangeWatcher(),
             SIGNAL( dateChanged() ),
             SLOT( slotStandardTimeSpansChanged() ) );

    // load settings:
    QSettings settings;
    if ( settings.contains( MetaKey_TimesheetActiveOnly ) ) {
        m_ui->checkBoxActiveOnly->setChecked( settings.value( MetaKey_TimesheetActiveOnly ).toBool() );
    } else {
        m_ui->checkBoxActiveOnly->setChecked( true );
    }
}

WeeklyTimesheetConfigurationDialog::~WeeklyTimesheetConfigurationDialog()
{
    delete m_ui; m_ui = 0;
}

void WeeklyTimesheetConfigurationDialog::setDefaultWeek(int yearOfWeek, int week)
{
    m_ui->spinBoxWeek->setValue(week);
    m_ui->spinBoxYear->setValue(yearOfWeek);
    m_ui->comboBoxWeek->setCurrentIndex(4);
}

void WeeklyTimesheetConfigurationDialog::accept()
{
    // save settings:
    QSettings settings;
    settings.setValue( MetaKey_TimesheetActiveOnly,
                       m_ui->checkBoxActiveOnly->isChecked() );
    settings.setValue( MetaKey_TimesheetRootTask,
                       m_rootTask );

    QDialog::accept();
}

void WeeklyTimesheetConfigurationDialog::showReportPreviewDialog( QWidget* parent )
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
    report->show();
}

void WeeklyTimesheetConfigurationDialog::showEvent( QShowEvent* )
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

void WeeklyTimesheetConfigurationDialog::slotCheckboxSubtasksOnlyChecked( bool checked )
{
    if ( checked && m_rootTask == 0 ) {
        slotSelectTask();
    }

    if ( ! checked ) {
        m_rootTask = 0;
        m_ui->labelTaskName->setText( tr( "(All Tasks)" ) );
    }
}

void WeeklyTimesheetConfigurationDialog::slotStandardTimeSpansChanged()
{
    const TimeSpans timeSpans;
    m_weekInfo = timeSpans.last4Weeks();
    NamedTimeSpan custom = {
        tr( "Manual Selection" ),
        timeSpans.thisWeek().timespan
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

void WeeklyTimesheetConfigurationDialog::slotWeekComboItemSelected( int index )
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

void WeeklyTimesheetConfigurationDialog::slotSelectTask()
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

    QPushButton* upload = uploadButton();
    connect(upload, SIGNAL(clicked()), SLOT(slotUploadTimesheet()) );
    if (!HttpJob::credentialsAvailable())
        upload->hide();
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
    Q_FOREACH( const TaskId i, childIds ) {
        children << taskWithSubTasks( i, m_secondsMap, &myInformation );
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
    const EventIdList matchingEvents = DATAMODEL->eventsThatStartInTimeFrame( m_start, m_end );

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
            if (i % 2)
                row.setAttribute( "class", "alternate_row" );
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
    uploadButton()->setEnabled(true);
}

void  WeeklyTimeSheetReport::slotSaveToXml()
{
    qDebug() << "WeeklyTimeSheet::slotSaveToXml: creating XML time sheet";
    // first, ask for a file name:
    QString filename = getFileName( tr("Charm reports (*.charmreport)") );
    if (filename.isEmpty())
        return;

    QFileInfo fileinfo( filename );
    if ( fileinfo.suffix().isEmpty() ) {
        filename += QLatin1String( ".charmreport" );
    }

    QByteArray payload = saveToXml();
    if (payload.isEmpty())
        return; // Error should have been already displayed by saveToXml()

    QFile file( filename );
    if ( file.open( QIODevice::WriteOnly ) ) {
        file.write( payload );
    } else {
        QMessageBox::critical( this, tr( "Error saving report" ),
                               tr( "Cannot write to selected location." ) );
    }
}

QString WeeklyTimeSheetReport::suggestedFileName() const
{
    return tr( "WeeklyTimeSheet-%1-%2" ).arg( m_yearOfWeek ).arg( m_weekNumber, 2, 10, QChar('0') );
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
    path += QDir::separator() + suggestedFileName();
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

QByteArray WeeklyTimeSheetReport::saveToXml()
{
    try {
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
            EventIdList matchingEvents = DATAMODEL->eventsThatStartInTimeFrame( m_start, m_end );
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

//     qDebug() << "WeeklyTimeSheetReport::slotSaveToXml: generated XML:" << endl
//              << document.toString( 4 );
//
       return document.toByteArray( 4 );
    } catch ( XmlSerializationException& e ) {
        QMessageBox::critical( this, tr( "Error exporting the report" ), e.what() );
    }

    return QByteArray();
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
                               tr( "Cannot write to selected location:\n%1" )
                               .arg( file.errorString() ) );
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

void WeeklyTimeSheetReport::slotUploadTimesheet()
{
    UploadTimesheetJob* client = new UploadTimesheetJob( this );
    connect( client, SIGNAL(finished(HttpJob*)), this, SLOT(slotTimesheetUploaded(HttpJob*)) );
    client->setParentWidget( this );
    client->setFileName( suggestedFileName() );
    client->setPayload( saveToXml() );
    client->start();
    uploadButton()->setEnabled(false);
}

void WeeklyTimeSheetReport::slotTimesheetUploaded(HttpJob* client) {

    if ( client->error() == HttpJob::Canceled ) {
        uploadButton()->setEnabled(true);
        return;
    }
    if ( client->error()  ) {
        uploadButton()->setEnabled(true);
        QMessageBox::critical(this, tr("Error"), tr("Could not upload timesheet: %1").arg( client->errorString() ) );
    }
    else
    {
        addUploadedTimesheet(m_yearOfWeek, m_weekNumber);
        QMessageBox::information(this, tr("Timesheet Uploaded"), tr("Your timesheet was successfully uploaded."));
    }
}

#include "WeeklyTimesheet.moc"
