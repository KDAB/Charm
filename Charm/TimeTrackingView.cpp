#include "ViewHelpers.h"

#include "TimeTrackingView.h"
#include "TimeTrackingSummaryWidget.h"
#include "Reports/CharmReport.h"

#include "ui_TimeTrackingView.h"

TimeTrackingView::TimeTrackingView( QWidget* parent )
    : QWidget( parent )
    , m_ui( new Ui::TimeTrackingView )
{
    m_ui->setupUi( this );
}


TimeTrackingView::~TimeTrackingView()
{
    delete m_ui; m_ui = 0;
}

TimeTrackingSummaryWidget* TimeTrackingView::summaryWidget()
{
    Q_ASSERT( m_ui );
    return m_ui->summaryWidget;
}

void TimeTrackingView::stateChanged( State previous )
{
    if ( Application::instance().state() == Connected ) {

        // temp:
        TimeTrackingSummaryWidget::WeeklySummary s;
        QVector<TimeTrackingSummaryWidget::WeeklySummary> summaries;
        summaries << s << s << s << s;
        summaries[0].task = 1;
        summaries[1].task = 240;
        summaries[2].task = 7102;
        summaries[3].task = 662;
        // add some random durations:
        summaries[0].durations[0] = 7230; // seconds, monday, first task
        summaries[0].durations[3] = 17230;
        summaries[1].durations[1] = 99999;
        summaries[1].durations[4] = 4567;
        summaries[2].durations[2] = 267;
        summaries[3].durations[5] = 120;
        summaries[3].durations[6] = 8765;
        for ( int i = 0; i < summaries.count(); ++i ) {
            const Task& task = DATAMODEL->getTask( summaries[i].task );
            summaries[i].taskname = tasknameWithParents( task );
        }
        // s1.durations << 1000 << 1100 << 1200 << 1300 << 1400 << 1500 << 1600;
        summaryWidget()->setSummaries( summaries );
    }

}

void TimeTrackingView::saveConfiguration()
{
}

void TimeTrackingView::emitCommand( CharmCommand* )
{
}

void TimeTrackingView::sendCommand( CharmCommand* )
{
}

void TimeTrackingView::commitCommand( CharmCommand* )
{
}

void TimeTrackingView::restore()
{
}

void TimeTrackingView::quit()
{
}

#include "TimeTrackingView.moc"
