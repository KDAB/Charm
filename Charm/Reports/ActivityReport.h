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
