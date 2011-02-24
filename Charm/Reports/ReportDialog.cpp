#include <QCloseEvent>

#include "ReportDialog.h"
#include "ui_ReportDialog.h"

// the "well known" report types (as opposed to possible plugin based
// report types:
#include "ActivityReport.h"
#include "WeeklyTimeSheet.h"

ReportDialog::ReportDialog( QWidget* parent )
    : QDialog( parent )
    , m_ui( new Ui::ReportDialog )
{
    m_ui->setupUi( this );
    // add all reports:
    m_reports << new WTSConfigurationPage( this )
              << new ActivityReportConfigurationPage( this );
    // FEATURE: add report plugins (NI)

    // initialize GUI with all the report types:
    for ( int i = 0; i < m_reports.size(); ++i )
    {
        m_ui->stackedWidget->addWidget( m_reports[i] );
        m_ui->comboReportSelector->addItem( m_reports[i]->name() );
        // add pages for the reports:
        m_ui->stackedWidget->addWidget( m_reports[i] );
    }

    connect( m_ui->pushButtonCancel, SIGNAL( clicked() ),
             SLOT( reject() ) );
}

ReportDialog::~ReportDialog()
{
    delete m_ui; m_ui = 0;
}

// void ReportDialog::showEvent( QShowEvent* )
// {
//     m_ui->stackedWidget->setCurrentWidget( m_ui->startPage );
//     emit visible( true );
// }

// void ReportDialog::closeEvent( QCloseEvent* event )
// {
//     event->setAccepted( false );
//     reject();
// }

// void ReportDialog::reject()
// {
//     emit visible( false );
//     QDialog::reject();
// }

void ReportDialog::on_comboReportSelector_currentIndexChanged( int index )
{
    m_ui->labelDescription->setText( m_reports[index]->description() );
}

void ReportDialog::on_pushButtonNext_clicked()
{
    m_ui->stackedWidget->setCurrentWidget( m_reports[m_ui->comboReportSelector->currentIndex()] );
}

void ReportDialog::back()
{
    m_ui->stackedWidget->setCurrentWidget( m_ui->startPage );
}

// CharmReport* ReportDialog::selectedReport()
// {
//     Q_ASSERT( m_ui->comboReportSelector->currentIndex() != -1 );
//     Q_ASSERT( m_ui->comboReportSelector->currentIndex() >= 0 );
//     Q_ASSERT( m_ui->comboReportSelector->currentIndex() < m_reports.size() );
//     return m_reports[m_ui->comboReportSelector->currentIndex()];
// }

ReportConfigurationPage* ReportDialog::selectedPage()
{
    Q_ASSERT( m_ui->comboReportSelector->currentIndex() != -1 );
    Q_ASSERT( m_ui->comboReportSelector->currentIndex() >= 0 );
    Q_ASSERT( m_ui->comboReportSelector->currentIndex() < m_reports.size() );
    return m_reports[m_ui->comboReportSelector->currentIndex()];
}

#include "ReportDialog.moc"



