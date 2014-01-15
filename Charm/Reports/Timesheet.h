#ifndef TIMESHEET3_H
#define TIMESHEET3_H

#include <Core/Task.h>

#include "ReportPreviewWindow.h"

class TimeSheetReport : public ReportPreviewWindow
{
    Q_OBJECT

public:
    explicit TimeSheetReport( QWidget* parent = nullptr );
    virtual ~TimeSheetReport();

    virtual void setReportProperties( const QDate& start,
                                      const QDate& end,
                                      TaskId rootTask,
                                      bool activeTasksOnly );

    typedef QMap< TaskId, QVector<int> > SecondsMap;

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

    inline SecondsMap & secondsMap()
        { return m_secondsMap; }

    QString getFileName( const QString& filter );

    // reimpl
    void slotUpdate();
    // reimpl
    void slotSaveToText();
    // reimpl
    void slotSaveToXml();

private:
    // properties of the report:
    QDate m_start;
    QDate m_end;
    TaskId m_rootTask;
    bool m_activeTasksOnly;
    SecondsMap m_secondsMap;
};

class TimeSheetInfo;
typedef QList<TimeSheetInfo> TimeSheetInfoList;

class TimeSheetInfo
{
public:
    TimeSheetInfo(int segments);
    int total() const;
    void dump();

public:
    static TimeSheetInfoList taskWithSubTasks( int segments, TaskId id, const TimeSheetReport::SecondsMap& secondsMap, TimeSheetInfo* addTo = nullptr );
    static TimeSheetInfoList filteredTaskWithSubTasks( TimeSheetInfoList timeSheetInfo, bool activeTasksOnly );

public:
    // the level of indentation, >0 means the numbers are aggregated for the subtasks:
    int indentation;
    QString taskname;
    QVector<int> seconds;
    TaskId taskId;
    bool aggregated;
};

#endif
