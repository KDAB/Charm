#include "WeeklyTimesheet.h"

#include <QCalendarWidget>
#include <QFile>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>

#include <Core/Dates.h>
#include <Core/XmlSerialization.h>

#include "DateEntrySyncer.h"
#include "HttpClient/UploadTimesheetJob.h"
#include "SelectTaskDialog.h"
#include "ViewHelpers.h"

#include "CharmCMake.h"

#include "ui_WeeklyTimesheetConfigurationDialog.h"

namespace {
    static const char * SETTING_GRP_TIMESHEETS = "timesheets";
    static const char * SETTING_VAL_FIRSTYEAR = "firstYear";
    static const char * SETTING_VAL_FIRSTWEEK = "firstWeek";
    static const int MAX_WEEK = 53;
    static const int MIN_YEAR = 1990;
    static const int DaysInWeek = 7;

    enum TimeSheetTableColumns {
        Column_Task,
        Column_Monday,
        Column_Tuesday,
        Column_Wednesday,
        Column_Thursday,
        Column_Friday,
        Column_Saturday,
        Column_Sunday,
        Column_Total,
        NumberOfColumns
    };
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
        int lastWeekOfYear = iYear == year ? week - 1 : Charm::numberOfWeeksInYear(iYear);
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

/************************************************** WeeklyTimesheetConfigurationDialog */

WeeklyTimesheetConfigurationDialog::WeeklyTimesheetConfigurationDialog( QWidget* parent )
    : ReportConfigurationDialog( parent )
    , m_ui( new Ui::WeeklyTimesheetConfigurationDialog )
{
    setWindowTitle( tr( "Weekly Timesheet" ) );

    m_ui->setupUi( this );
    m_ui->dateEditDay->calendarWidget()->setFirstDayOfWeek( Qt::Monday );
    m_ui->dateEditDay->calendarWidget()->setVerticalHeaderFormat( QCalendarWidget::ISOWeekNumbers );
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
        end = start.addDays( DaysInWeek );
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

/*************************************************************** WeeklyTimeSheetReport */
// here begins ... the actual report:

WeeklyTimeSheetReport::WeeklyTimeSheetReport( QWidget* parent )
    : TimeSheetReport( parent )
    , m_weekNumber( 0 )
    , m_yearOfWeek( 0 )
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
    m_weekNumber = start.weekNumber( &m_yearOfWeek );
    TimeSheetReport::setReportProperties(start, end, rootTask, activeTasksOnly);
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

void WeeklyTimeSheetReport::slotTimesheetUploaded(HttpJob* client)
{
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

QString WeeklyTimeSheetReport::suggestedFileName() const
{
    return tr( "WeeklyTimeSheet-%1-%2" ).arg( m_yearOfWeek ).arg( m_weekNumber, 2, 10, QChar('0') );
}

void WeeklyTimeSheetReport::update()
{   // this creates the time sheet
    // retrieve matching events:
    const EventIdList matchingEvents = DATAMODEL->eventsThatStartInTimeFrame( startDate(), endDate() );

    secondsMap().clear();

    // for every task, make a vector that includes a number of seconds
    // for every day of the week ( int seconds[7]), and store those in
    // a map by their task id
    Q_FOREACH( EventId id, matchingEvents ) {
        const Event& event = DATAMODEL->eventForId( id );
        QVector<int> seconds( DaysInWeek );
        if ( secondsMap().contains( event.taskId() ) ) {
            seconds = secondsMap().value(event.taskId());
        }
        // what day in the week is the event (normalized to vector indexes):
        int dayOfWeek = event.startDateTime().date().dayOfWeek() - 1;
        Q_ASSERT( dayOfWeek >= 0 && dayOfWeek < DaysInWeek );
        seconds[dayOfWeek] += event.duration();
        // store in minute map:
        secondsMap()[event.taskId()] = seconds;
    }
    // now the reporting:
    // headline first:
    QTextDocument report;
    QDomDocument doc = createReportTemplate();
    QDomElement root = doc.documentElement();
    QDomElement body = root.firstChildElement( "body" );

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
        // TimeSheetInfoList timeSheetInfo = taskWithSubTasks( rootTask(), secondsMap() );
        TimeSheetInfoList timeSheetInfo = TimeSheetInfo::filteredTaskWithSubTasks(
            TimeSheetInfo::taskWithSubTasks( DaysInWeek, rootTask(), secondsMap() ),
            activeTasksOnly() );

        QDomElement table = doc.createElement( "table" );
        table.setAttribute( "width", "100%" );
        table.setAttribute( "align", "left" );
        table.setAttribute( "cellpadding", "3" );
        table.setAttribute( "cellspacing", "0" );
        body.appendChild( table );

        TimeSheetInfo totalsLine(DaysInWeek);
        if ( ! timeSheetInfo.isEmpty() ) {
            totalsLine = timeSheetInfo.first();
            if( rootTask() == 0 ) {
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
            tr( "%1" ).arg( startDate().day(), 2, 10, QLatin1Char('0') ),
            tr( "%1" ).arg( startDate().addDays( 1 ).day(), 2, 10, QLatin1Char('0') ),
            tr( "%1" ).arg( startDate().addDays( 2 ).day(), 2, 10, QLatin1Char('0') ),
            tr( "%1" ).arg( startDate().addDays( 3 ).day(), 2, 10, QLatin1Char('0') ),
            tr( "%1" ).arg( startDate().addDays( 4 ).day(), 2, 10, QLatin1Char('0') ),
            tr( "%1" ).arg( startDate().addDays( 5 ).day(), 2, 10, QLatin1Char('0') ),
            tr( "%1" ).arg( startDate().addDays( 6 ).day(), 2, 10, QLatin1Char('0') ),
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
            report.setDefaultStyleSheet( style );
        } else {
            qDebug() << "WeeklyTimeSheet::create: default style sheet is empty, too bad";
        }
    } else {
        qDebug() << "WeeklyTimeSheet::create: cannot load report style sheet:" << stylesheet.errorString();
    }

    report.setHtml( doc.toString() );
    setDocument( &report );
    uploadButton()->setEnabled(true);
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
        TimeSheetInfoList timeSheetInfo = TimeSheetInfo::filteredTaskWithSubTasks(
            TimeSheetInfo::taskWithSubTasks( DaysInWeek, rootTask(), m_secondsMap ),
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

//     qDebug() << "WeeklyTimeSheetReport::slotSaveToXml: generated XML:" << endl
//              << document.toString( 4 );
//
        return document.toByteArray( 4 );
    } catch ( const XmlSerializationException& e ) {
        QMessageBox::critical( this, tr( "Error exporting the report" ), e.what() );
    }

    return QByteArray();
}

QByteArray WeeklyTimeSheetReport::saveToText()
{
    QByteArray output;
    QTextStream stream( &output );
    QString content = tr( "Report for %1, Week %2 (%3 to %4)" )
                      .arg( CONFIGURATION.user.name() )
                      .arg( m_weekNumber, 2, 10, QChar('0') )
                      .arg( startDate().toString( Qt::TextDate ) )
                      .arg( endDate().addDays( -1 ).toString( Qt::TextDate ) );
    stream << content << '\n';
    stream << '\n';
    TimeSheetInfoList timeSheetInfo = TimeSheetInfo::filteredTaskWithSubTasks(
        TimeSheetInfo::taskWithSubTasks( DaysInWeek, rootTask(), secondsMap() ),
        activeTasksOnly() );

    TimeSheetInfo totalsLine( DaysInWeek );
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
    stream << "Week total: " << hoursAndMinutes( totalsLine.total() ) << '\n';
    stream.flush();

    return output;
}

