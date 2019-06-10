/*
  MonthlyTimesheet.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2014-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

#ifndef MONTHLYTIMESHEET3_H
#define MONTHLYTIMESHEET3_H

#include <Core/Task.h>

#include "Timesheet.h"

class QUrl;

class MonthlyTimeSheetReport : public TimeSheetReport
{
    Q_OBJECT

public:
    explicit MonthlyTimeSheetReport(QWidget *parent = nullptr);
    ~MonthlyTimeSheetReport() override;

    void setReportProperties(const QDate &start, const QDate &end, TaskId rootTask,
                             bool activeTasksOnly) override;

private Q_SLOTS:
    void updateRange(int deltaMonths);

private:
    QString suggestedFileName() const override;
    void update() override;
    QByteArray saveToText() override;
    QByteArray saveToXml(SaveToXmlMode mode) override;

private:
    // properties of the report:
    int m_numberOfWeeks = 0;
    int m_monthNumber = 0;
    int m_yearOfMonth = 0;
    QString m_weeklyhours;
    float m_dailyhours;
};

#endif
