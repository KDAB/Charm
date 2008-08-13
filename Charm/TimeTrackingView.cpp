#include "TimeTrackingView.h"
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

