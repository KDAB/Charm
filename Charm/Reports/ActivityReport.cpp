#include <QFile>
#include <QTimer>
#include <QSettings>
#include <QDomElement>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>
#include <QtAlgorithms>
#include <QCalendarWidget>
#include <QDomDocument>
#include <QPushButton>

#include <Core/Configuration.h>
#include <Core/Dates.h>

#include <QDialogButtonBox>
#include <QTimer>

#include "Application.h"
#include "ViewHelpers.h"
#include "CharmReport.h"
#include "SelectTaskDialog.h"
#include "ActivityReport.h"
#include "DateEntrySyncer.h"

#include "ui_ActivityReportConfigurationDialog.h"

ActivityReportConfigurationDialog::ActivityReportConfigurationDialog( QWidget* parent )
    : ReportConfigurationDialog( parent )
    , m_ui( new Ui::ActivityReportConfigurationDialog )
    , m_rootTask( 0 )
{
    setWindowTitle( tr( "Activity Report" ) );

    m_ui->setupUi( this );
    m_ui->dateEditEnd->calendarWidget()->setFirstDayOfWeek( Qt::Monday );
    m_ui->dateEditEnd->calendarWidget()->setVerticalHeaderFormat( QCalendarWidget::ISOWeekNumbers );
    m_ui->dateEditStart->calendarWidget()->setFirstDayOfWeek( Qt::Monday );
    m_ui->dateEditStart->calendarWidget()->setVerticalHeaderFormat( QCalendarWidget::ISOWeekNumbers );

    connect( m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()) );
    connect( m_ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()) );
    connect( m_ui->comboBox, SIGNAL( currentIndexChanged( int ) ),
             SLOT( slotTimeSpanSelected( int ) ) );
    connect( m_ui->checkBoxSubTasksOnly, SIGNAL( toggled( bool ) ),
             SLOT( slotCheckboxSubtasksOnlyChecked( bool ) ) );
    connect( m_ui->checkBoxExcludeTasks, SIGNAL( toggled( bool ) ),
             SLOT( slotCheckBoxExcludeTasksChecked( bool ) ) );
    connect( m_ui->toolButtonSelectTask, SIGNAL( clicked() ),
             SLOT( slotSelectTask() ) );
    connect( m_ui->toolButtonExcludeTask, SIGNAL( clicked() ),
             SLOT( slotExcludeTask() ) );
    slotCheckboxSubtasksOnlyChecked( m_ui->checkBoxSubTasksOnly->isChecked() );
    slotCheckBoxExcludeTasksChecked( m_ui->checkBoxExcludeTasks->isChecked() );

    new DateEntrySyncer(m_ui->spinBoxStartWeek, m_ui->spinBoxStartYear, m_ui->dateEditStart, 1,  this );
    new DateEntrySyncer(m_ui->spinBoxEndWeek, m_ui->spinBoxEndYear, m_ui->dateEditEnd, 7, this );

    QTimer::singleShot( 0, this, SLOT( slotDelayedInitialization() ) );
}

ActivityReportConfigurationDialog::~ActivityReportConfigurationDialog()
{
    delete m_ui; m_ui = 0;
}

void ActivityReportConfigurationDialog::slotDelayedInitialization()
{
    slotStandardTimeSpansChanged();
    connect( Application::instance().dateChangeWatcher(),
             SIGNAL( dateChanged() ),
             SLOT( slotStandardTimeSpansChanged() ) );
    // FIXME load settings
}

void ActivityReportConfigurationDialog::slotStandardTimeSpansChanged()
{
    const TimeSpans timeSpans;
    m_timespans = timeSpans.standardTimeSpans();
    NamedTimeSpan customRange = {
        tr( "Select Range" ),
        timeSpans.thisWeek().timespan
    };
    m_timespans << customRange;
    m_ui->comboBox->clear();
    for ( int i = 0; i < m_timespans.size(); ++i )
    {
        m_ui->comboBox->addItem( m_timespans[i].name );
    }
}

void ActivityReportConfigurationDialog::slotTimeSpanSelected( int index )
{
    if ( m_ui->comboBox->count() == 0 || index == -1 ) return;
    Q_ASSERT( m_ui->comboBox->count() > index );
    if ( index >= m_timespans.size() - 2 ) { // manual selection
        m_ui->groupBox->setEnabled( true );
    } else {
        m_ui->spinBoxStartYear->setValue( m_timespans[index].timespan.first.year() );
        m_ui->spinBoxStartWeek->setValue( m_timespans[index].timespan.first.weekNumber() );
        m_ui->spinBoxEndYear->setValue( m_timespans[index].timespan.second.year() );
        m_ui->spinBoxEndWeek->setValue( m_timespans[index].timespan.second.weekNumber() );
        m_ui->dateEditStart->setDate( m_timespans[index].timespan.first );
        m_ui->dateEditEnd->setDate( m_timespans[index].timespan.second );
        m_ui->groupBox->setEnabled( false );
    }
}

void ActivityReportConfigurationDialog::slotCheckboxSubtasksOnlyChecked( bool checked )
{
    if ( checked && m_rootTask == 0 ) {
        slotSelectTask();
    }

    if ( ! checked ) {
        m_rootTask = 0;
        m_ui->labelTaskName->setText( tr( "(All Tasks)" ) );
    }
}

void ActivityReportConfigurationDialog::slotCheckBoxExcludeTasksChecked( bool checked )
{
    if ( checked && m_rootExcludeTask == 0 ) {
        slotExcludeTask();
    }

    if ( ! checked ) {
        m_rootExcludeTask = 0;
        m_ui->labelExcludeTaskName->setText( tr( "(No Tasks)" ) );
    }
}

void ActivityReportConfigurationDialog::slotSelectTask()
{
    if ( selectTask( m_rootTask ) ) {
        const TaskTreeItem& item = DATAMODEL->taskTreeItem( m_rootTask );
        m_ui->labelTaskName->setText( DATAMODEL->fullTaskName( item.task() ) );
    } else {
        if ( m_rootTask == 0 )
            m_ui->checkBoxSubTasksOnly->setChecked( false );
    }
}

void ActivityReportConfigurationDialog::slotExcludeTask()
{
    if ( selectTask( m_rootExcludeTask ) ) {
        const TaskTreeItem& item = DATAMODEL->taskTreeItem( m_rootExcludeTask );
        m_ui->labelExcludeTaskName->setText( DATAMODEL->fullTaskName( item.task() ) );
    } else {
        if ( m_rootExcludeTask == 0 )
            m_ui->checkBoxExcludeTasks->setChecked( false );
    }
}

bool ActivityReportConfigurationDialog::selectTask(TaskId& task)
{
    SelectTaskDialog dialog( this );
    const bool taskSelected = dialog.exec();
    if ( taskSelected )
        task = dialog.selectedTask();
    return taskSelected;
}

void ActivityReportConfigurationDialog::accept()
{
    // FIXME save settings
    QDialog::accept();
}

void ActivityReportConfigurationDialog::showReportPreviewDialog( QWidget* parent )
{
    QDate start, end;
    const int index = m_ui->comboBox->currentIndex();
    if ( index == m_timespans.size() - 1 ) { //Range
        start = m_ui->dateEditStart->date();
        end = m_ui->dateEditEnd->date().addDays( 1 );
    } else {
        start = m_timespans[index].timespan.first;
        end = m_timespans[index].timespan.second;
    }

    ActivityReport* report = new ActivityReport( parent );
    report->setReportProperties( start, end, m_rootTask, m_rootExcludeTask );
    report->show();
}

ActivityReport::ActivityReport( QWidget* parent )
    : ReportPreviewWindow( parent )
{
    saveToXmlButton()->hide();
    saveToTextButton()->hide();
    uploadButton()->hide();
}

ActivityReport::~ActivityReport()
{
}

void ActivityReport::setReportProperties(
    const QDate& start, const QDate& end, TaskId rootTask, TaskId rootExcludeTask )
{
    m_start = start;
    m_end = end;
    m_rootTask = rootTask;
    m_rootExcludeTask = rootExcludeTask;
    slotUpdate();
}

void ActivityReport::slotUpdate()
{
    const QString DateFormat( "yyyy/MM/dd" );
    const QString TimeFormat( "HH:mm" );
    const QString DateTimeFormat( "yyyy/MM/dd HH:mm" );

    // retrieve matching events:
    EventIdList matchingEvents = DATAMODEL->eventsThatStartInTimeFrame( m_start, m_end );
    matchingEvents = eventIdsSortedByStartTime( matchingEvents );
    if ( m_rootTask != 0 ) {
        matchingEvents = filteredBySubtree( matchingEvents, m_rootTask );
    }

    // filter unproductive events:
    if ( m_rootExcludeTask != 0 ) {
        matchingEvents = filteredBySubtree( matchingEvents, m_rootExcludeTask, true );
    }

    // calculate total:
    int totalSeconds = 0;
    Q_FOREACH( EventId id, matchingEvents ) {
        const Event& event = DATAMODEL->eventForId( id );
        Q_ASSERT( event.isValid() );
        totalSeconds += event.duration();
    }

    QTextDocument* report = new QTextDocument( this );
    QDomDocument doc = createReportTemplate();
    QDomElement root = doc.documentElement();
    QDomElement body = root.firstChildElement( "body" );

    // create the caption:
    {
        QDomElement headline = doc.createElement( "h1" );
        QDomText text = doc.createTextNode( tr( "Activity Report" ) );
        headline.appendChild( text );
        body.appendChild( headline );
    }
    {
        QDomElement headline = doc.createElement( "h3" );
        QString content = tr( "Report for %1, from %2 to %3" )
                          .arg( CONFIGURATION.user.name() )
                          .arg( m_start.toString( Qt::TextDate ) )
                          .arg( m_end.toString( Qt::TextDate ) );
        QDomText text = doc.createTextNode( content );
        headline.appendChild( text );
        body.appendChild( headline );
        {
            QDomElement paragraph = doc.createElement( "h4" );
            QString totalsText = tr( "Total: %1" ).arg( hoursAndMinutes( totalSeconds ) );
            QDomText totalsElement = doc.createTextNode( totalsText );
            paragraph.appendChild( totalsElement );
            body.appendChild( paragraph );
        }
        if ( m_rootTask != 0 ) {
            QDomElement paragraph = doc.createElement( "p" );
            const Task& task = DATAMODEL->getTask( m_rootTask );
            QString rootTaskText = tr( "Activity under task %1" ).arg( DATAMODEL->fullTaskName( task ) );
            QDomText rootText = doc.createTextNode( rootTaskText );
            paragraph.appendChild( rootText );
            body.appendChild( paragraph );
        }

        QDomElement paragraph = doc.createElement( "br" );
        body.appendChild( paragraph );
    }
    {
        const QString Headlines[] = {
            tr( "Date and Time, Task, Description" )
        };
        const int NumberOfColumns = sizeof Headlines / sizeof Headlines[0];

        // now for a table
        QDomElement table = doc.createElement( "table" );
        table.setAttribute( "width", "100%" );
        table.setAttribute( "align", "left" );
        table.setAttribute( "cellpadding", "3" );
        table.setAttribute( "cellspacing", "0" );
        body.appendChild( table );
        // table header
        QDomElement tableHead = doc.createElement( "thead" );
        table.appendChild( tableHead );
        QDomElement headerRow = doc.createElement( "tr" );
        headerRow.setAttribute( "class", "header_row" );
        tableHead.appendChild( headerRow );
        // column headers
        for ( int i = 0; i < NumberOfColumns; ++i )
        {
            QDomElement header = doc.createElement( "th" );
            QDomText text = doc.createTextNode( Headlines[i] );
            header.appendChild( text );
            headerRow.appendChild( header );
        }
        QDomElement tableBody = doc.createElement( "tbody" );
        table.appendChild( tableBody );
        // rows
        Q_FOREACH( EventId id, matchingEvents ) {
            const Event& event = DATAMODEL->eventForId( id );
            Q_ASSERT( event.isValid() );
            const TaskTreeItem& item = DATAMODEL->taskTreeItem( event.taskId() );
            const Task& task = item.task();
            Q_ASSERT( task.isValid() );

            const QString row1Texts[] = {
                tr( "%1 %2-%3 (%4) -- [%5] %6" )
                .arg( event.startDateTime().date().toString( Qt::SystemLocaleShortDate ).trimmed() )
                .arg( event.startDateTime().time().toString( Qt::SystemLocaleShortDate ).trimmed() )
                .arg( event.endDateTime().time().toString( Qt::SystemLocaleShortDate ).trimmed() )
                .arg( hoursAndMinutes( event.duration() ) )
                .arg( QString().setNum( task.id() ).trimmed(), Configuration::instance().taskPaddingLength, '0' )
                .arg( task.name().trimmed() )
            };

            QDomElement row1 = doc.createElement( "tr" );
            row1.setAttribute( "class", "event_attributes_row" );
            QDomElement row2 = doc.createElement( "tr" );
            for ( int index = 0; index < NumberOfColumns; ++index ) {
                QDomElement cell = doc.createElement( "td" );
                cell.setAttribute( "class", "event_attributes" );
                QDomText text = doc.createTextNode( row1Texts[index] );
                cell.appendChild( text );
                row1.appendChild( cell );
            }
            QDomElement cell2 = doc.createElement( "td" );
            cell2.setAttribute( "class", "event_description" );
            cell2.setAttribute( "align", "left" );
            QDomElement preElement = doc.createElement( "pre" );
            QDomText preText = doc.createTextNode( event.comment() );
            preElement.appendChild( preText );
            cell2.appendChild( preElement );
            row2.appendChild( cell2 );

            tableBody.appendChild( row1 );
            tableBody.appendChild( row2 );
        }
    }

    // NOTE: seems like the style sheet has to be set before the html
    // code is pushed into the QTextDocument
    QFile stylesheet( ":/Charm/report_stylesheet.sty" );
    if ( stylesheet.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
        QString style = stylesheet.readAll();
        if ( !style.isEmpty() ) {
            report->setDefaultStyleSheet( style );
        } else {
            qDebug() << "WeeklyTimeSheet::create: default style sheet is empty, too bad";
        }
    } else {
        qDebug() << "WeeklyTimeSheet::create: cannot load report style sheet: " << stylesheet.errorString();
    }

    report->setHtml( doc.toString() );
    setDocument( report );
}

#include "ActivityReport.moc"
