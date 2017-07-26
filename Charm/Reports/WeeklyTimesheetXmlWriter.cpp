/*
  WeeklyTimesheetXmlWriter.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2014-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

#include "WeeklyTimesheetXmlWriter.h"
#include "TimesheetInfo.h"

#include <QDomDocument>

WeeklyTimesheetXmlWriter::WeeklyTimesheetXmlWriter()
    : TimesheetXmlWriter(QLatin1String("weekly-timesheet"))
{
}

void WeeklyTimesheetXmlWriter::setYear(int year)
{
    m_year = year;
}

void WeeklyTimesheetXmlWriter::setWeekNumber(int weekNumber)
{
    m_weekNumber = weekNumber;
}

void WeeklyTimesheetXmlWriter::writeMetadata(QDomDocument &document, QDomElement &metadata) const
{
    // extend metadata tag: add year, and serial (week) number:
    QDomElement yearElement = document.createElement(QStringLiteral("year"));
    metadata.appendChild(yearElement);
    QDomText text = document.createTextNode(QString::number(m_year));
    yearElement.appendChild(text);
    QDomElement weekElement = document.createElement(QStringLiteral("serial-number"));
    weekElement.setAttribute(QStringLiteral("semantics"), QStringLiteral("week-number"));
    metadata.appendChild(weekElement);
    QDomText weektext = document.createTextNode(QString::number(m_weekNumber));
    weekElement.appendChild(weektext);
}

QList<TimeSheetInfo> WeeklyTimesheetXmlWriter::createTimeSheetInfo() const
{
    static const int DaysInWeek = 7;
    return TimeSheetInfo::filteredTaskWithSubTasks(
            TimeSheetInfo::taskWithSubTasks(dataModel(), DaysInWeek, rootTask(), SecondsMap()),
            false);  // here, we don't care about active or not, because we only report on the tasks
}
