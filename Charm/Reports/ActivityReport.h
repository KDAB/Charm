#ifndef ACTIVITYREPORT_H
#define ACTIVITYREPORT_H

#include <Core/Task.h>
#include <Core/TimeSpans.h>

#include "ReportConfigurationPage.h"
#include "ReportPreviewWindow.h"

namespace Ui {
    class ActivityReportConfigurationPage;
}

class ActivityReportConfigurationPage : public ReportConfigurationPage
{
    Q_OBJECT

public:
    explicit ActivityReportConfigurationPage( ReportDialog* parent );
    ~ActivityReportConfigurationPage();

    QDialog* makeReportPreviewDialog( QWidget* parent );
    QString name();
    QString description();

private slots:
    void slotDelayedInitialization();
    void slotOkClicked();
    void slotStandardTimeSpansChanged();
    void slotTimeSpanSelected( int );
    void slotCheckboxSubtasksOnlyChecked( bool );
    void slotCheckBoxExcludeTasksChecked( bool );
    void slotSelectTask();
    void slotExcludeTask();

private:
    bool selectTask(TaskId& task);

    Ui::ActivityReportConfigurationPage* m_ui;
    QList<NamedTimeSpan> m_timespans;
    TaskId m_rootTask;
    TaskId m_rootExcludeTask;
};

class ActivityReport : public ReportPreviewWindow
{
    Q_OBJECT

public:
    explicit ActivityReport( QWidget* parent = 0 );
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
