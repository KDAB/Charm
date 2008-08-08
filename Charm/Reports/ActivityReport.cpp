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

#include <QTimer>

#include "Application.h"
#include "ViewHelpers.h"
#include "ActivityReport.h"
#include "ui_ActivityReportConfigurationPage.h"

ActivityReportConfigurationPage::ActivityReportConfigurationPage( ReportDialog* parent )
    : ReportConfigurationPage( parent )
    , m_ui( new Ui::ActivityReportConfigurationPage )
{
    m_ui->setupUi( this );
    connect( m_ui->pushButtonBack, SIGNAL( clicked() ),
             SIGNAL( back() ) );
    connect( m_ui->pushButtonReport, SIGNAL( clicked() ),
             SLOT( slotOkClicked() ) );
    connect( m_ui->comboBox, SIGNAL( currentIndexChanged( int ) ),
             SLOT( slotTimeSpanSelected( int ) ) );
    QTimer::singleShot( 0, this, SLOT( slotDelayedInitialization() ) );
}

QString ActivityReportConfigurationPage::name()
{
    return QObject::tr( "Activity Report" );
}

QString ActivityReportConfigurationPage::description()
{
    return QObject::tr( "Create a printable report on all your activity within "
                        "a certain time frame. The time frame could be a day, a "
                        "week or any other period." );
}

void ActivityReportConfigurationPage::slotDelayedInitialization()
{
    slotStandardTimeSpansChanged();
    connect( &Application::instance().timeSpans(),
             SIGNAL( timeSpansChanged() ),
             SLOT( slotStandardTimeSpansChanged() ) );
    // FIXME load settings
}

void ActivityReportConfigurationPage::slotStandardTimeSpansChanged()
{
    m_timespans = Application::instance().timeSpans().standardTimeSpans();
    NamedTimeSpan custom = {
        tr( "Manual Selection" ),
        Application::instance().timeSpans().thisWeek().timespan
    };
    m_timespans << custom;
    m_ui->comboBox->clear();
    for ( int i = 0; i < m_timespans.size(); ++i )
    {
        m_ui->comboBox->addItem( m_timespans[i].name );
    }
}

void ActivityReportConfigurationPage::slotTimeSpanSelected( int index )
{
    if ( m_ui->comboBox->count() == 0 || index == -1 ) return;
    Q_ASSERT( m_ui->comboBox->count() > index );
    if ( index == m_timespans.size() -1 ) { // manual selection
        m_ui->groupBox->setEnabled( true );
    } else {
        m_ui->dateEditStart->setDate( m_timespans[index].timespan.first );
        m_ui->dateEditEnd->setDate( m_timespans[index].timespan.second );
        m_ui->groupBox->setEnabled( false );
    }
}

void ActivityReportConfigurationPage::slotOkClicked()
{
    // FIXME save settings
    emit accept();
}

QDialog* ActivityReportConfigurationPage::makeReportPreviewDialog( QWidget* parent )
{
    QDate start, end;
    int index = m_ui->comboBox->currentIndex();
    if ( index == m_timespans.size() -1 ) {
        // manual selection
        start = m_ui->dateEditStart->date();
        end = m_ui->dateEditEnd->date();
    } else {
        start = m_timespans[index].timespan.first;
        end = m_timespans[index].timespan.second;
    }
    ActivityReport* report = new ActivityReport( parent );
    report->setReportProperties( start, end );
    return report;
}

ActivityReport::ActivityReport( QWidget* parent )
    : ReportPreviewWindow( parent )
{
}

ActivityReport::~ActivityReport()
{
}

void ActivityReport::setReportProperties( const QDate& start, const QDate& end )
{
    m_start = start;
    m_end = end;
    slotUpdate();
}

void ActivityReport::slotUpdate()
{
    const QString DateFormat( "yyyy/MM/dd" );
    const QString TimeFormat( "HH:mm" );
    const QString DateTimeFormat( "yyyy/MM/dd HH:mm" );

    // retrieve matching events:
    EventIdList matchingEvents = DATAMODEL->eventsThatStartInTimeFrame(
        QDateTime( m_start ), QDateTime( m_end ) );
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
        QDomElement paragraph = doc.createElement( "br" );
        body.appendChild( paragraph );
    }
    {
        const QString Headlines[] = {
            tr( "Task Id" ),
            tr( "Task Name" ),
            tr( "Date" ),
            tr( "Start" ),
            tr( "End" ),
            tr( "Duration" ),
            tr( "Comment" )
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

            const QString rowTexts[] = {
                QString().setNum( task.id() ),
                task.name(),
                event.startDateTime().date().toString( Qt::SystemLocaleShortDate ),
                event.startDateTime().time().toString( Qt::SystemLocaleShortDate ),
                event.endDateTime().time().toString( Qt::SystemLocaleShortDate ),
                hoursAndMinutes( event.duration() ),
                event.comment()
            };
            Q_ASSERT( sizeof rowTexts / sizeof rowTexts[0] == NumberOfColumns );

            QDomElement row = doc.createElement( "tr" );
            for ( int index = 0; index < NumberOfColumns; ++index ) {
                QDomElement cell = doc.createElement( "td" );
                cell.setAttribute( "align", "left" );
                QDomText text = doc.createTextNode( rowTexts[index] );
                cell.appendChild( text );
                row.appendChild( cell );
            }
            tableBody.appendChild( row );
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
        qDebug() << "WeeklyTimeSheet::create: cannot load report style sheet";
    }

    report->setHtml( doc.toString() );
    setDocument( report );
}

#include "ActivityReport.moc"
