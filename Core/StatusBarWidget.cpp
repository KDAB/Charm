#include "StatusBarWidget.h"
#include "ui_StatusBarWidget.h"

StatusBarWidget::StatusBarWidget( QWidget* parent )
    : QWidget( parent )
    , m_ui( new Ui::StatusBarWidget )
{
    m_ui->setupUi( this );
    m_ui->progressBar->hide();
}

StatusBarWidget::~StatusBarWidget()
{
    delete m_ui;
}

#include "StatusBarWidget.moc"
