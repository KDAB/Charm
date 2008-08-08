#include <QTimer>

#include "Application.h"
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
}

#include "ActivityReport.moc"
