#ifndef ACTIVITYREPORT_H
#define ACTIVITYREPORT_H

#include <QDate>

#include "Core/Event.h"
#include "CharmReport.h"
#include "Core/TimeSpans.h"

#include "ui_ActivityReportConfigurationPage.h"

class QTextDocument;

class ActivityReport : public CharmReport
{
    Q_OBJECT

public:
    explicit ActivityReport( QObject* parent = 0 );

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
    Ui::ActivityReportConfigurationPage m_ui;
};

#endif
