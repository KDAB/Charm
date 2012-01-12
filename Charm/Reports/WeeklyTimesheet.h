#ifndef WEEKLYTIMESHEET3_H
#define WEEKLYTIMESHEET3_H

#include <Core/Task.h>
#include <Core/TimeSpans.h>

#include "ReportPreviewWindow.h"
#include "ReportConfigurationDialog.h"

namespace Ui {
    class WeeklyTimesheetConfigurationDialog;
}

class HttpJob;

class WeeklyTimesheetConfigurationDialog : public ReportConfigurationDialog
{
    Q_OBJECT

public:
    explicit WeeklyTimesheetConfigurationDialog( QWidget* parent );
    ~WeeklyTimesheetConfigurationDialog();

    QDialog* makeReportPreviewDialog( QWidget* parent );

    void showEvent( QShowEvent* );

public Q_SLOTS:
    void accept();

private slots:
    void slotDelayedInitialization();
    void slotCheckboxSubtasksOnlyChecked( bool );
    void slotStandardTimeSpansChanged();
    void slotWeekComboItemSelected( int );
    void slotSelectTask();

private:
    Ui::WeeklyTimesheetConfigurationDialog* m_ui;
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
                              TaskId rootTask,
                              bool activeTasksOnly );

    typedef QMap< TaskId, QVector<int> > SecondsMap;

private slots:

    void slotUploadTimesheet();
    void slotTimesheetUploaded(HttpJob*);
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

    QString getFileName( const QString& filter );
    QByteArray saveToXml();

    // reimpl
    void slotUpdate();
    // reimpl
    void slotSaveToXml();
    // reimpl
    void slotSaveToText();

    // properties of the report:
    QDate m_start;
    QDate m_end;
    int m_weekNumber;
    int m_yearOfWeek;
    TaskId m_rootTask;
    bool m_activeTasksOnly;
    QTextDocument* m_report; // FIXME unnecessary
    SecondsMap m_secondsMap;
};

#endif
