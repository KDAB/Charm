/*
  TimesheetInfo.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Frank Osterfeld <frank.osterfeld@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
    QString formattedTaskIdAndName( int taskPaddingLength ) const;

    // the level of indentation, >0 means the numbers are aggregated for the subtasks:
    int indentation;
    QString taskName;
    QVector<int> seconds;
    TaskId taskId;
    bool aggregated;
};

#endif
