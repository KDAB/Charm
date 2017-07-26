/*
  MonthlyTimesheetXmlWriter.cpp

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

#include "MonthlyTimesheetXmlWriter.h"

#include "TimesheetInfo.h"
#include <QDomDocument>

MonthlyTimesheetXmlWriter::MonthlyTimesheetXmlWriter()
    : TimesheetXmlWriter(QLatin1String("monthly-timesheet"))
{
}

void MonthlyTimesheetXmlWriter::setYearOfMonth(int yearOfMonth)
{
    m_yearOfMonth = yearOfMonth;
}

void MonthlyTimesheetXmlWriter::setMonthNumber(int monthNumber)
{
    m_monthNumber = monthNumber;
}

void MonthlyTimesheetXmlWriter::setNumberOfWeeks(int numberOfWeeks)
{
    m_numberOfWeeks = numberOfWeeks;
}

void MonthlyTimesheetXmlWriter::writeMetadata(QDomDocument &document, QDomElement &metadata) const
{
    QDomElement yearElement = document.createElement(QStringLiteral("year"));
    metadata.appendChild(yearElement);
    QDomText text = document.createTextNode(QString::number(m_yearOfMonth));
    yearElement.appendChild(text);
    QDomElement monthElement = document.createElement(QStringLiteral("serial-number"));
    monthElement.setAttribute(QStringLiteral("semantics"), QStringLiteral("month-number"));
    metadata.appendChild(monthElement);
    QDomText monthtext = document.createTextNode(QString::number(m_monthNumber));
    monthElement.appendChild(monthtext);
}

QList<TimeSheetInfo> MonthlyTimesheetXmlWriter::createTimeSheetInfo() const
{
    return TimeSheetInfo::filteredTaskWithSubTasks(
        TimeSheetInfo::taskWithSubTasks(dataModel(), m_numberOfWeeks, rootTask(), SecondsMap()),
       false);  // here, we don't care about active or not, because we only report on the tasks
}
