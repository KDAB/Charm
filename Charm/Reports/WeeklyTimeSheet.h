#ifndef WEEKLYTIMESHEET_H
#define WEEKLYTIMESHEET_H

#include <QDateTime>
#include <QDomDocument>

#include "CharmReport.h"

#include "ui_WeeklyTimeSheetConfigurationPage.h"

class QWidget;
class ReportDialog;

class WeeklyTimeSheet : public CharmReport
{
    Q_OBJECT

public:
    explicit WeeklyTimeSheet( QObject* parent = 0 );

    bool prepare();

    bool create();

    QTextDocument* report();

    QString description();

    QString name();

    QWidget* configurationPage( ReportDialog* );

private slots:
    void slotDelayedInitialization();
    void slotWeekComboItemSelected( int );
    void slotSelectTask();
    void slotCheckboxSubtasksOnlyChecked( bool );
    void slotStandardTimeSpansChanged();

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

    // this may be elevated to a default document generator:
    QDomDocument createReportTemplate();

    QList<NamedTimeSpan> m_weekInfo;
    EventIdList m_matchingEvents;
    QDate m_start;
    QDate m_end;
    int m_weekNumber;
    TaskId m_rootTask;

    QWidget* m_configurationPage;
    Ui::WeeklyTimeSheetConfigurationPage m_ui;
    QTextDocument* m_report;
};

#endif
