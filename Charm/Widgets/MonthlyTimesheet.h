#ifndef MONTHLYTIMESHEET3_H
#define MONTHLYTIMESHEET3_H

#include <Core/Task.h>

#include "Timesheet.h"

class MonthlyTimeSheetReport : public TimeSheetReport
{
    Q_OBJECT

public:
    explicit MonthlyTimeSheetReport( QWidget* parent = 0 );
    virtual ~MonthlyTimeSheetReport();

    // reimpl
    void setReportProperties( const QDate& start,
                              const QDate& end,
                              TaskId rootTask,
                              bool activeTasksOnly );

    typedef QMap< TaskId, QVector<int> > SecondsMap;

private: // reimpl
    QString suggestedFileName() const;
    void update();
    QByteArray saveToText();
    QByteArray saveToXml();

private:
    // properties of the report:
    int m_numberOfWeeks;
    int m_monthNumber;
    int m_yearOfMonth;
};

#endif
