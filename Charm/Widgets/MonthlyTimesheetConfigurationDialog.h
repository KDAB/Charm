#ifndef MONTHLYTIMESHEETCONFIGURATIONDIALOG_H
#define MONTHLYTIMESHEETCONFIGURATIONDIALOG_H

#include <Core/Task.h>
#include <Core/TimeSpans.h>

#include "ReportConfigurationDialog.h"

#include <QScopedPointer>

namespace Ui { class MonthlyTimesheetConfigurationDialog; }

class MonthlyTimesheetConfigurationDialog : public ReportConfigurationDialog
{
    Q_OBJECT

public:
    explicit MonthlyTimesheetConfigurationDialog( QWidget* parent );
    virtual ~MonthlyTimesheetConfigurationDialog();

    void showReportPreviewDialog( QWidget* parent );
    void showEvent( QShowEvent* );
    void setDefaultMonth( int yearOfMonth, int month );

public Q_SLOTS:
    void accept();

private slots:
    void slotCheckboxSubtasksOnlyChecked( bool );
    void slotStandardTimeSpansChanged();
    void slotMonthComboItemSelected( int );
    void slotSelectTask();

private:
    QScopedPointer<Ui::MonthlyTimesheetConfigurationDialog> m_ui;
    QList<NamedTimeSpan> m_monthInfo;
    TaskId m_rootTask;
};

#endif
