#ifndef MONTHLYTIMESHEET3_H
#define MONTHLYTIMESHEET3_H

#include <Core/Task.h>

#include "ReportPreviewWindow.h"

typedef QHash<int, QVector<int> > WeeksByYear;

class MonthlyTimeSheetReport : public ReportPreviewWindow
{
    Q_OBJECT

public:
    explicit MonthlyTimeSheetReport( QWidget* parent = 0 );
    virtual ~MonthlyTimeSheetReport();

    void setReportProperties( const QDate& start,
                              const QDate& end,
                              TaskId rootTask,
                              bool activeTasksOnly );

    typedef QMap< TaskId, QVector<int> > SecondsMap;

private:

    enum TimeSheetTableColumns {
        Column_Task,
        Column_Week1,
        Column_Week2,
        Column_Week3,
        Column_Week4,
        Column_Week5,
        Column_Total,
        Column_Days,
        NumberOfColumns
    };

    QString suggestedFileName() const;
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
    int m_monthNumber;
    int m_yearOfMonth;
    TaskId m_rootTask;
    bool m_activeTasksOnly;
    QTextDocument* m_report; // FIXME unnecessary
    SecondsMap m_secondsMap;
};

#endif
