/*
  WeeklyTimesheetXmlWriter.h

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

#ifndef WEEKLYTIMESHEETXMLWRITER_H
#define WEEKLYTIMESHEETXMLWRITER_H

#include "Core/Event.h"
#include "Core/Task.h"

class QByteArray;
class CharmDataModel;

class WeeklyTimesheetXmlWriter {
public:
    WeeklyTimesheetXmlWriter();
    /**
     * @throws XmlSerializationException
     */
    QByteArray saveToXml() const;

    void setDataModel( const CharmDataModel* model);
    void setYear( int year );
    void setWeekNumber( int weekNumber );
    void setEvents( const EventList& events );
    void setRootTask( TaskId rootTask );
private:
    const CharmDataModel* m_dataModel;
    int m_year;
    int m_weekNumber;
    TaskId m_rootTask;
    EventList m_events;
};

#endif
