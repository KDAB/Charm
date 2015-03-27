/*
  Timesheet.h

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

#ifndef TIMESHEET3_H
#define TIMESHEET3_H

#include <Core/Task.h>

#include "ReportPreviewWindow.h"
#include "Reports/TimesheetInfo.h"

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

    void slotUpdate() override;
    void slotSaveToText() override;
    void slotSaveToXml() override;

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
