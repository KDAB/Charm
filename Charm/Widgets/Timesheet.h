#ifndef TIMESHEET3_H
#define TIMESHEET3_H

#include <Core/Task.h>

#include "ReportPreviewWindow.h"
#include "Reports/TimesheetInfo.h"

class TimeSheetReport : public ReportPreviewWindow
{
    Q_OBJECT

public:
    explicit TimeSheetReport( QWidget* parent = 0 );
    virtual ~TimeSheetReport();

    virtual void setReportProperties( const QDate& start,
                                      const QDate& end,
                                      TaskId rootTask,
                                      bool activeTasksOnly );

protected:

    virtual QString suggestedFileName() const = 0;
    virtual void update() = 0;
    virtual QByteArray saveToText() = 0;
    virtual QByteArray saveToXml() = 0;

protected:

    inline QDate startDate() const
        { return m_start; }

    inline QDate endDate() const
        { return m_end; }

    inline TaskId rootTask() const
        { return m_rootTask; }

    inline bool activeTasksOnly() const
        { return m_activeTasksOnly; }

    inline const SecondsMap &secondsMap() const
        { return m_secondsMap; }

    QString getFileName( const QString& filter );

    // reimpl
    void slotUpdate();
    // reimpl
    void slotSaveToText();
    // reimpl
    void slotSaveToXml();

protected:
    SecondsMap m_secondsMap;

private:
    // properties of the report:
    QDate m_start;
    QDate m_end;
    TaskId m_rootTask;
    bool m_activeTasksOnly;
};

#endif
