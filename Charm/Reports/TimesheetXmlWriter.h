/*
  TimesheetXmlWriter.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2014-2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

#ifndef TIMESHEETXMLWRITER_H
#define TIMESHEETXMLWRITER_H

#include <QList>

#include "Core/Event.h"
#include "Core/Task.h"

class QByteArray;
class QDomDocument;
class QDomElement;

class CharmDataModel;
class TimeSheetInfo;

class TimesheetXmlWriter {
public:
    explicit TimesheetXmlWriter(const QString &templateName);
    virtual ~TimesheetXmlWriter();

    const CharmDataModel *dataModel() const;
    void setDataModel(const CharmDataModel *model);

    TaskId rootTask() const;
    void setRootTask(TaskId rootTask);

    bool includeTaskList() const;
    void setIncludeTaskList(bool);

    /**
     * @throws XmlSerializationException
     */
    QByteArray saveToXml() const;

    EventList events() const;
    void setEvents(const EventList &events);

protected:
    virtual void writeMetadata(QDomDocument &document, QDomElement &metadata) const = 0;
    virtual QList<TimeSheetInfo> createTimeSheetInfo() const = 0;

private:
    const CharmDataModel *m_dataModel = nullptr;
    EventList m_events;
    TaskId m_rootTask = {};
    QString m_templateName;
    bool m_includeTaskList = true;
};

#endif
