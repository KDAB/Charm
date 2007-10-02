#include <algorithm>

#include <QTimer>
#include <QWidget>
#include <QDateTime>
#include <QTextStream>
#include <QPushButton>
#include <QTextBlock>
#include <QTextDocument>
#include <QTextTableCell>

#include "Core/Event.h"
#include "Core/CharmConstants.h"
#include "ViewHelpers.h"
#include "Application.h"
#include "ReportDialog.h"
#include "Core/Configuration.h"
#include "ActivityReport.h"
#include "ReportPreviewWindow.h"
#include "ParagraphFormatCollection.h"

ActivityReport::ActivityReport( QObject* parent )
    : CharmReport( parent )
    , m_report( 0 )
    , m_configurationPage( 0 )
{
}

QString ActivityReport::name()
{
    return QObject::tr( "Activity Report" );
}

QString ActivityReport::description()
{
    return QObject::tr( "Create a printable report on all your activity within "
                        "a certain time frame. The time frame could be a day, a "
                        "week or any other period." );
}

bool ActivityReport::prepare()
{
    m_start = m_ui.dateEditStart->dateTime();
    m_end = m_ui.dateEditEnd->dateTime();
    m_matchingEvents = eventsThatStartInTimeFrame( m_start, m_end );

    qDebug() << "ActivityReport::prepare:" << m_matchingEvents.size() << "events";
    qStableSort( m_matchingEvents.begin(), m_matchingEvents.end(),
                 StartsEarlier );

    return CharmReport::prepare();
}

bool ActivityReport::create()
{
    m_report = new QTextDocument;

    const QString DateFormat( "yyyy/MM/dd" );
    const QString TimeFormat( "HH:mm" );
    const QString DateTimeFormat( "yyyy/MM/dd HH:mm" );

    QTextCursor cursor( m_report );

    // create the caption:
    {
        paragraphFormats().format( "Headline" ).apply( cursor );

        QString headline;
        QTextStream stream( &headline );
        stream << "Activity Report for "
               << CONFIGURATION.user.name() << endl;

        cursor.insertText( headline );
    }
    // opening information
    {
        paragraphFormats().format( "default" ).apply( cursor );

        QString opener;
        QTextStream stream( &opener );
        stream << "From " << m_start.toString( DateTimeFormat )
               << " to " << m_end.toString( DateTimeFormat ) << endl;

        cursor.insertText( opener );
    }
    // create section with activity information:
    {
        paragraphFormats().format( "Headline1" ).apply( cursor );
        cursor.insertText( "Activities" );
        cursor.insertBlock();

        paragraphFormats().format( "default" ).apply( cursor );
        cursor.insertText( "All activity, sorted by start time." );

        // now for a table
        // rows: one per event, plus header and summary
        const int Rows = m_matchingEvents.size() + 2;
        QTextTableFormat tableFormat;
        // tableFormat.setBackground( QColor( "#e0e0e0" ) );
 //        QVector<QTextLength> constraints;
//         constraints << QTextLength( QTextLength::PercentageLength, 10 );
//         constraints << QTextLength( QTextLength::PercentageLength, 40 );
//         constraints << QTextLength( QTextLength::PercentageLength, 13 );
//         constraints << QTextLength( QTextLength::PercentageLength, 7 );
//         constraints << QTextLength( QTextLength::PercentageLength, 30 );
//         tableFormat.setColumnWidthConstraints( constraints );
        tableFormat.setWidth( QTextLength( QTextLength::PercentageLength, 100 ) );
        tableFormat.setCellSpacing( 6 );
        tableFormat.setCellPadding( 0 );
        tableFormat.setHeaderRowCount( 1 );
        tableFormat.setBorder( 0 );
        tableFormat.setMargin( 12 );
// tableFormat.setBackground( QBrush( "lightgray" ) );
        QTextTable *table = cursor.insertTable( Rows, NumberOfColumns,
                                                tableFormat );

        QTextTableCell cell;
        QTextCursor cellCursor;
        const QString Headlines[NumberOfColumns] = {
            tr( "Task Id" ),
            tr( "Task Name" ),
            tr( "Date" ),
            tr( "Start Time" ),
            tr( "End Time" ),
            tr( "Duration" ),
            tr( "Comment" )
        };

        int grandTotal = 0;

        for ( int i = 0; i < NumberOfColumns; ++i )
        {
            cell = table->cellAt( 0, i );
            cellCursor = cell.firstCursorPosition();
            paragraphFormats().format( "table-header" ).apply( cellCursor );
            cellCursor.insertText( Headlines[i] );
        }

        for ( int row = 1; row < Rows - 1; ++row )
        {
            const Event& event = DATAMODEL->eventForId( m_matchingEvents[row - 1] );
            Task task = DATAMODEL->getTask( event.taskId() );
            QString texts[NumberOfColumns];

            grandTotal += event.duration();

            // create the cell values:
            texts[Column_TaskId] = QString("%1" ).arg(
                task.id(),
                CONFIGURATION.taskPaddingLength,
                10, QChar( '0' ) );

            texts[Column_TaskName] = tasknameWithParents( task );

            texts[Column_Date] = event.startDateTime().toString( DateFormat );

            texts[Column_StartTime] = event.startDateTime().toString( TimeFormat );

            texts[Column_EndTime] = event.endDateTime().toString( TimeFormat );

            texts[Column_Duration] = hoursAndMinutes( event.duration() );

            texts[Column_Comment] = ( event.comment().isEmpty() ? tr( "-" ) : event.comment() );

            for ( int column = 0; column < NumberOfColumns; ++column )
            {
                cell = table->cellAt( row, column );
                cellCursor = cell.firstCursorPosition();
//                 paragraphFormats().format( row % 2 ? "table-data-odd" : "table-data-even" )
//                     .apply( cellCursor );
                paragraphFormats().format( "table-data" ).apply( cellCursor );
                cellCursor.insertText( texts[column] );
            }
        }
        // put summary:
        cell = table->cellAt( Rows - 1, Column_EndTime );
        cellCursor  = cell.firstCursorPosition();
        paragraphFormats().format( "table-header" ).apply( cellCursor );
        cellCursor.insertText( tr( "Total:" ) );
        cell = table->cellAt( Rows - 1, Column_Duration );
        cellCursor = cell.firstCursorPosition();
        paragraphFormats().format( "table-header" ).apply( cellCursor );
        cellCursor.insertText( hoursAndMinutes( grandTotal ) );
    }

    return true;
}

QTextDocument* ActivityReport::report()
{
    return m_report;
}

QWidget* ActivityReport::configurationPage( ReportDialog* dialog )
{
    if ( m_configurationPage == 0 ) {
        m_configurationPage = new QWidget;
        m_ui.setupUi( m_configurationPage );

        connect( m_ui.pushButtonBack, SIGNAL( clicked() ),
                 dialog, SLOT( back() ) );

        connect( m_ui.comboBox, SIGNAL( currentIndexChanged( int ) ),
                 SLOT( timeFrameChanged( int ) ) );

        connect( m_ui.pushButtonReport, SIGNAL( clicked() ),
                 SLOT( makeReportPreviewWindow() ) );

        QTimer::singleShot( 0, this, SLOT( delayedInitialization() ) );
        m_ui.dateEditStart->setCalendarPopup( true );
        m_ui.dateEditEnd->setCalendarPopup( true );
    }

    return m_configurationPage;
}

void ActivityReport::timeFrameChanged( int index )
{
    // wait for the next update, in this case:
    if ( m_ui.comboBox->count() == 0 || index == -1 ) return;
    Q_ASSERT( m_ui.comboBox->count() > index );

    if ( index == m_timeSpans.size() -1 ) { // manual selection
        // this is the custom time frame item
        m_ui.groupBox->setEnabled( true );
    } else {
        m_ui.groupBox->setEnabled( false );
        m_ui.dateEditStart->setDate( m_timeSpans[index].timespan.first );
        m_ui.dateEditEnd->setDate( m_timeSpans[index].timespan.second );
    }
    // otherwise: ignore (this combobox is never cleared, except when
    // updated, so there will be a new index changed signal as soon as
    // the new contents are set
}

void ActivityReport::delayedInitialization()
{
    standardTimeSpansChanged();
    connect( &Application::instance().timeSpans(),
             SIGNAL( timeSpansChanged() ),
             SLOT( standardTimeSpansChanged() ) );
}

void ActivityReport::standardTimeSpansChanged()
{
    m_timeSpans = Application::instance().timeSpans().standardTimeSpans();
    NamedTimeSpan custom = {
        tr( "Manual Selection" ),
        Application::instance().timeSpans().today().timespan
    };
    m_timeSpans << custom;

    m_ui.comboBox->clear();
    for ( int i = 0; i < m_timeSpans.size(); ++i )
    {
        m_ui.comboBox->addItem( m_timeSpans[i].name );
    }
}

#include "ActivityReport.moc"

