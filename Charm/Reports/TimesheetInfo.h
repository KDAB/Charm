#ifndef TIMESHEETINFO_H
#define TIMESHEETINFO_H

#include <QList>
#include <QString>
#include <QVector>

#include "Core/Task.h"

class CharmDataModel;
class TimeSheetInfo;
typedef QList<TimeSheetInfo> TimeSheetInfoList;

typedef QMap< TaskId, QVector<int> > SecondsMap;

class TimeSheetInfo
{
public:
    explicit TimeSheetInfo( int segments );
    int total() const;
    void dump();

public:
    static TimeSheetInfoList taskWithSubTasks( const CharmDataModel* dataModel, int segments, TaskId id, const SecondsMap& secondsMap, TimeSheetInfo* addTo = 0 );
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
