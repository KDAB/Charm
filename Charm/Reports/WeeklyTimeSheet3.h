#ifndef WEEKLYTIMESHEET3_H
#define WEEKLYTIMESHEET3_H

#include <Core/Task.h>
#include <Core/TimeSpans.h>

#include "ReportPreviewWindow.h"
#include "ReportConfigurationPage.h"

namespace Ui {
    class WeeklyTimeSheetConfigurationPage;
}

class WTSConfigurationPage : public ReportConfigurationPage
{
    Q_OBJECT

public:
    explicit WTSConfigurationPage( ReportDialog* parent );
    ~WTSConfigurationPage();

    QDialog* makeReportPreviewDialog( QWidget* parent );
    QString name();
    QString description();

private slots:
    void slotDelayedInitialization();
    void slotCheckboxSubtasksOnlyChecked( bool );
    void slotStandardTimeSpansChanged();
    void slotWeekComboItemSelected( int );
    void slotSelectTask();

private:
    Ui::WeeklyTimeSheetConfigurationPage* m_ui;
    QList<NamedTimeSpan> m_weekInfo;
    TaskId m_rootTask;
};

class WeeklyTimeSheetReport : public ReportPreviewWindow
{
    Q_OBJECT

public:
    explicit WeeklyTimeSheetReport( QWidget* parent = 0 );
    ~WeeklyTimeSheetReport();

    void setReportProperties( const QDate& start,
                              const QDate& end,
                              int weekNumber,
                              TaskId rootTask,
                              bool activeTasksOnly,
                              bool subscribedOnly );

private:
        enum TimeSheetTableColumns {
        Column_Task,
        Column_Monday,
        Column_Tuesday,
        Column_Wednesday,
        Column_Thursday,
        Column_Friday,
        Column_Saturday,
        Column_Sunday,
        Column_Total,
        NumberOfColumns
    };

    // reimpl
    void slotUpdate();

    // properties of the report:
    QDate m_start;
    QDate m_end;
    int m_weekNumber;
    TaskId m_rootTask;
    bool m_activeTasksOnly;
    bool m_subscribedOnly;
    QTextDocument* m_report; // FIXME unnecessary
};

#endif
