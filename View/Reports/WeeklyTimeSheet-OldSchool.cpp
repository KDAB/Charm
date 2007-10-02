#include <QMap>
#include <QTimer>
#include <QVector>
#include <QTextBlock>
#include <QTextDocument>
#include <QTextTableCell>

#include "Core/CharmConstants.h"

#include "ViewHelpers.h"
#include "ReportDialog.h"
#include "SelectTaskDialog.h"
#include "WeeklyTimeSheet-OldSchool.h"
#include "ReportPreviewWindow.h"
#include "ParagraphFormatCollection.h"

WeeklyTimeSheet_OldSchool::WeeklyTimeSheet_OldSchool( QObject* parent )
    : CharmReport( parent )
    , m_rootTask( 0 )
    , m_configurationPage( 0 )
    , m_report( 0 )
{
}

QString WeeklyTimeSheet_OldSchool::name()
{
    return QObject::tr( "Weekly Time Sheet - Old School" );
}

QString WeeklyTimeSheet_OldSchool::description()
{
    return QObject::tr
        ( "Creates a tabular report on all activity within a week. "
          "The report is summarized by task and by day. Does not "
          "really look fancy, though. Have a look at the other time "
          "sheet." );
}

bool WeeklyTimeSheet_OldSchool::prepare()
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

    qDebug() << "WeeklyTimeSheet_OldSchool::prepare: start:"  << m_start
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
    // FIXME sort by task id
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

bool WeeklyTimeSheet_OldSchool::create()
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
    QTextCursor cursor( m_report );
    TimeSheetInfo totalsLine;
    // create the caption:
    {
        paragraphFormats().format( "Headline" ).apply( cursor );

        QString headline;
        QTextStream stream( &headline );
        stream << "Weekly Time Sheet" << endl;

        cursor.insertText( headline );
    }
    {
        paragraphFormats().format( "Headline1" ).apply( cursor );
        QString headline;
        QTextStream stream( &headline );
        stream << "Report for " << CONFIGURATION.user.name()
               << ", Week" << m_weekNumber << endl;

        cursor.insertText( headline );
    }
    {
        // now for a table
        // retrieve the information for the report:
        TimeSheetInfoList timeSheetInfo = taskWithSubTasks( m_rootTask, secondsMap );

        if ( m_ui.checkBoxActiveOnly->isChecked() ) {
            TimeSheetInfoList nonZero;
            // FIXME use algorithm (I just hate to slug the fat book around)
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

        if ( ! timeSheetInfo.isEmpty() ) {
            totalsLine = timeSheetInfo.first();
            if( m_rootTask == 0 ) {
                timeSheetInfo.removeAt( 0 ); // there is always one, because there is always the root item
            }
        }

        const int Rows = timeSheetInfo.size() + 2; // + header row, totals
        QTextTableFormat tableFormat;
        tableFormat.setWidth( QTextLength( QTextLength::PercentageLength, 100 ) );
        tableFormat.setCellSpacing( 6 );
        tableFormat.setCellPadding( 1 );
        tableFormat.setHeaderRowCount( 1 );
        tableFormat.setBorder( 0 );
        tableFormat.setMargin( 12 );
        QTextTable *table = cursor.insertTable( Rows, NumberOfColumns,
                                                tableFormat );

        QTextTableCell cell;
        QTextCursor cellCursor;
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

        for ( int i = 0; i < NumberOfColumns; ++i )
        {
            cell = table->cellAt( 0, i );
            cellCursor = cell.firstCursorPosition();
            paragraphFormats().format( "table-header" ).apply( cellCursor );
            cellCursor.insertText( Headlines[i] );
        }

        int row = 1;
        // temp: this needs to be hierarchically sorted by tasks:
        for ( int i = 0; i < timeSheetInfo.size(); ++i )
        {
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
                cell = table->cellAt( row, column );
                cellCursor = cell.firstCursorPosition();
                if ( timeSheetInfo[i].aggregated ) {
                    paragraphFormats().format( "table-data-italic" ).apply( cellCursor );
                } else {
                    paragraphFormats().format( "table-data" ).apply( cellCursor );
                }

                if ( column == Column_Task ) {
                    QTextBlockFormat blockFormat = cellCursor.blockFormat();
                    blockFormat.setLeftMargin( 9 * timeSheetInfo[i].indentation - 1 );
                    cellCursor.setBlockFormat( blockFormat );
                }
                cellCursor.insertText( texts[column] );
            }

            ++row;
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
        for ( int i = 0; i < NumberOfColumns; ++i )
        {
            cell = table->cellAt( row, i );
            cellCursor = cell.firstCursorPosition();
            paragraphFormats().format( "table-header" ).apply( cellCursor );
            cellCursor.insertText( TotalsTexts[i] );
        }
    }

    return true;
}

QTextDocument* WeeklyTimeSheet_OldSchool::report()
{
    return m_report;
}

QWidget* WeeklyTimeSheet_OldSchool::configurationPage( ReportDialog* dialog )
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

void WeeklyTimeSheet_OldSchool::slotDelayedInitialization()
{
    slotStandardTimeSpansChanged();
    connect( &Application::instance().timeSpans(),
             SIGNAL( timeSpansChanged() ),
             SLOT( slotStandardTimeSpansChanged() ) );
}

void WeeklyTimeSheet_OldSchool::slotStandardTimeSpansChanged()
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

void WeeklyTimeSheet_OldSchool::slotWeekComboItemSelected( int index )
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

void WeeklyTimeSheet_OldSchool::slotSelectTask()
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

void WeeklyTimeSheet_OldSchool::slotCheckboxSubtasksOnlyChecked( bool checked )
{
    if ( checked && m_rootTask == 0 ) {
        slotSelectTask();
    }

    if ( ! checked ) {
        m_rootTask = 0;
        m_ui.labelTaskName->setText( tr( "(All Tasks)" ) );
    }
}

#include "WeeklyTimeSheet-OldSchool.moc"

