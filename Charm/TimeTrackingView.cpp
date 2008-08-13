#include "ViewHelpers.h"

#include "TimeTrackingView.h"
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

void TimeTrackingView::stateChanged( State previous )
{
    if ( Application::instance().state() == Connected ) {
        // temp:
        WeeklySummary s1;
        s1.task = 1;
        s1.durations << 1000 << 1100 << 1200 << 1300 << 1400 << 1500 << 1600;
        const Task& task = DATAMODEL->getTask( s1.task );
        s1.taskname = tasknameWithParents( task );
        qDebug() << s1.taskname;
        m_summaries << s1 << s1 << s1 << s1;
        //
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

