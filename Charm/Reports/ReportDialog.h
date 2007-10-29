#ifndef REPORTDIALOG_H
#define REPORTDIALOG_H

#include <QDialog>

// #include "CharmReport.h"

namespace Ui {
    class ReportDialog;
}

class ReportConfigurationPage;

class ReportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReportDialog( QWidget* parent = 0 );
    ~ReportDialog();

    ReportConfigurationPage* selectedPage();

// CharmReport* selectedReport();
//     void showEvent( QShowEvent* );
//     void closeEvent( QCloseEvent* );
//     void reject();

public slots:
    void on_comboReportSelector_currentIndexChanged( int );
    void on_pushButtonNext_clicked();

    void back();

signals:
    void visible( bool );

private:
    Ui::ReportDialog* m_ui;
    QList<ReportConfigurationPage*> m_reports;
};

#endif
