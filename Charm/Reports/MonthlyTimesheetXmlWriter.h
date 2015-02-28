/*
  MonthlyTimesheetXmlWriter.h

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

#ifndef MONTHLYTIMESHEETXMLWRITER_H
#define MONTHLYTIMESHEETXMLWRITER_H

#include "Core/Event.h"
#include "Core/Task.h"

class QByteArray;
class CharmDataModel;

class MonthlyTimesheetXmlWriter {
public:
    MonthlyTimesheetXmlWriter();
    /**
     * @throws XmlSerializationException
     */
    QByteArray saveToXml() const;

    void setDataModel( const CharmDataModel* dataModel );
    void setYearOfMonth( int yearOfMonth );
    void setMonthNumber( int monthNumber );
    void setNumberOfWeeks( int numberOfWeeks );
    void setEvents( const EventList& events );
    void setRootTask( TaskId rootTask );

private:
    const CharmDataModel* m_dataModel;
    int m_yearOfMonth;
    int m_monthNumber;
    int m_numberOfWeeks;
    TaskId m_rootTask;
    EventList m_events;
};

#endif
