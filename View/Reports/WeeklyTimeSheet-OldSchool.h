#ifndef WEEKLYTIMESHEET_OLDSCHOOL_H
#define WEEKLYTIMESHEET_OLDSCHOOL_H

#include <QDateTime>

#include "Core/TimeSpans.h"

#include "CharmReport.h"
#include "ui_WeeklyTimeSheetConfigurationPage.h"

class QWidget;
class QTextDocument;
class ReportDialog;

class WeeklyTimeSheet_OldSchool : public CharmReport
{
    Q_OBJECT

public:
    explicit WeeklyTimeSheet_OldSchool( QObject* parent = 0 );

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
