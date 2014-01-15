#ifndef ACTIVITYREPORT_H
#define ACTIVITYREPORT_H

#include <Core/Task.h>
#include <Core/TimeSpans.h>

#include "ReportConfigurationDialog.h"
#include "ReportPreviewWindow.h"

namespace Ui {
    class ActivityReportConfigurationDialog;
}

class ActivityReportConfigurationDialog : public ReportConfigurationDialog
{
    Q_OBJECT

public:
    explicit ActivityReportConfigurationDialog( QWidget* parent );
    ~ActivityReportConfigurationDialog();

    void showReportPreviewDialog( QWidget* parent );

public Q_SLOTS:
    void accept();

private slots:
    void slotDelayedInitialization();
    void slotStandardTimeSpansChanged();
    void slotTimeSpanSelected( int );
    void slotCheckboxSubtasksOnlyChecked( bool );
    void slotCheckBoxExcludeTasksChecked( bool );
    void slotSelectTask();
    void slotExcludeTask();

private:
    bool selectTask(TaskId& task);

    Ui::ActivityReportConfigurationDialog* m_ui;
    QList<NamedTimeSpan> m_timespans;
    TaskId m_rootTask;
    TaskId m_rootExcludeTask;
};

class ActivityReport : public ReportPreviewWindow
{
    Q_OBJECT

public:
    explicit ActivityReport( QWidget* parent = nullptr );
    ~ActivityReport();

    void setReportProperties( const QDate& start, const QDate& end,
        TaskId rootTask, TaskId rootExcludeTask );

private:
    // reimpl
    void slotUpdate();

private:
    QDate m_start;
    QDate m_end;
    TaskId m_rootTask;
    TaskId m_rootExcludeTask;
};

#endif
