#ifndef WEEKLYTIMESHEET3_H
#define WEEKLYTIMESHEET3_H

#include <Core/Task.h>
#include <Core/TimeSpans.h>

#include "Timesheet.h"
#include "ReportConfigurationDialog.h"

namespace Ui {
    class WeeklyTimesheetConfigurationDialog;
}

class HttpJob;

typedef QHash<int, QVector<int> > WeeksByYear;
///Set the timesheet for the @param week of the @param year as having been uploaded
void addUploadedTimesheet(int year, int week);
///Get all missing timesheets
WeeksByYear missingTimeSheets();

class WeeklyTimesheetConfigurationDialog : public ReportConfigurationDialog
{
    Q_OBJECT

public:
    explicit WeeklyTimesheetConfigurationDialog( QWidget* parent );
    ~WeeklyTimesheetConfigurationDialog();

    void showReportPreviewDialog( QWidget* parent );
    void showEvent( QShowEvent* );
    void setDefaultWeek( int yearOfWeek, int week );

public Q_SLOTS:
    void accept();

private slots:
    void slotCheckboxSubtasksOnlyChecked( bool );
    void slotStandardTimeSpansChanged();
    void slotWeekComboItemSelected( int );
    void slotSelectTask();

private:
    Ui::WeeklyTimesheetConfigurationDialog* m_ui;
    QList<NamedTimeSpan> m_weekInfo;
    TaskId m_rootTask;
};

class WeeklyTimeSheetReport : public TimeSheetReport
{
    Q_OBJECT

public:
    explicit WeeklyTimeSheetReport( QWidget* parent = 0 );
    virtual ~WeeklyTimeSheetReport();

    // reimpl
    void setReportProperties( const QDate& start,
                              const QDate& end,
                              TaskId rootTask,
                              bool activeTasksOnly );

    typedef QMap< TaskId, QVector<int> > SecondsMap;

private slots:
    void slotUploadTimesheet();
    void slotTimesheetUploaded(HttpJob*);

private: // reimpl
    QString suggestedFileName() const;
    void update();
    QByteArray saveToXml();
    QByteArray saveToText();

private:
    // properties of the report:
    int m_weekNumber;
    int m_yearOfWeek;
};

#endif
