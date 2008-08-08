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

void ActivityReportConfigurationPage::slotOkClicked()
{
    // FIXME save settings
    emit accept();
}

QDialog* ActivityReportConfigurationPage::makeReportPreviewDialog( QWidget* parent )
{
    return 0;
}

#include "ActivityReport.moc"


