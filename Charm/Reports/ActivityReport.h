#ifndef ACTIVITYREPORT_H
#define ACTIVITYREPORT_H

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

    QDialog* makeReportPreviewDialog( QWidget* parent );
    QString name();
    QString description();

private slots:
    void slotDelayedInitialization();
    void slotOkClicked();
    void slotStandardTimeSpansChanged();
    void slotTimeSpanSelected( int );

private:
    Ui::ActivityReportConfigurationPage* m_ui;
    QList<NamedTimeSpan> m_timespans;

/*
    bool prepare();

    bool create();

    QTextDocument* report();

    QString description();

    QString name();

    QWidget* configurationPage( ReportDialog* );

private slots:
    void delayedInitialization();
    void timeFrameChanged( int index );
    void standardTimeSpansChanged();

private:
    QList<NamedTimeSpan> m_timeSpans;

    enum ActivityTableColumns {
        Column_TaskId,
        Column_TaskName,
        Column_Date,
        Column_StartTime,
        Column_EndTime,
        Column_Duration,
        Column_Comment,
        NumberOfColumns
    };

    QDateTime m_start;
    QDateTime m_end;
    QTextDocument* m_report;
    EventIdList m_matchingEvents;
    QWidget* m_configurationPage;
*/
};

class ActivityReport : public ReportPreviewWindow
{
    Q_OBJECT

public:
    explicit ActivityReport( QWidget* parent = 0 );
    ~ActivityReport();

    void setReportProperties( const QDate& start, const QDate& end );

private:
    // reimpl
    void slotUpdate();

private:
    QDate m_start;
    QDate m_end;
};

#endif
