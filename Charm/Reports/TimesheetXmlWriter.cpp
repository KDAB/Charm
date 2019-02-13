/*
  TimesheetXmlWriter.cpp

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

#include "TimesheetXmlWriter.h"
#include "TimesheetInfo.h"
#include "CharmCMake.h"

#include "Core/CharmDataModel.h"
#include "Core/CharmConstants.h"
#include "Core/XmlSerialization.h"

#include <QDomDocument>

TimesheetXmlWriter::TimesheetXmlWriter(const QString &templateName)
    : m_templateName(templateName)
{
}

TimesheetXmlWriter::~TimesheetXmlWriter()
{
}

const CharmDataModel *TimesheetXmlWriter::dataModel() const
{
    return m_dataModel;
}

void TimesheetXmlWriter::setDataModel(const CharmDataModel *model)
{
    m_dataModel = model;
}

EventList TimesheetXmlWriter::events() const
{
    return m_events;
}

void TimesheetXmlWriter::setEvents(const EventList &events)
{
    m_events = events;
}

TaskId TimesheetXmlWriter::rootTask() const
{
    return m_rootTask;
}

void TimesheetXmlWriter::setRootTask(TaskId rootTask)
{
    m_rootTask = rootTask;
}

bool TimesheetXmlWriter::includeTaskList() const
{
    return m_includeTaskList;
}

void TimesheetXmlWriter::setIncludeTaskList(bool includeTaskList)
{
    m_includeTaskList = includeTaskList;
}

QByteArray TimesheetXmlWriter::saveToXml() const
{
    // now create the report:
    QDomDocument document = XmlSerialization::createXmlTemplate(m_templateName);

    // find metadata and report element:
    QDomElement root = document.documentElement();
    QDomElement metadata = XmlSerialization::metadataElement(document);
    QDomElement charmVersion = document.createElement(QStringLiteral("charmversion"));
    QDomText charmVersionString = document.createTextNode(CharmVersion());
    charmVersion.appendChild(charmVersionString);
    metadata.appendChild(charmVersion);
    auto installationId = document.createElement(QStringLiteral("installation-id"));
    const auto installationIdString = document.createTextNode(QString::number(CONFIGURATION.installationId));
    installationId.appendChild(installationIdString);
    metadata.appendChild(installationId);

    QDomElement report = XmlSerialization::reportElement(document);
    Q_ASSERT(!root.isNull() && !metadata.isNull() && !report.isNull());

    writeMetadata(document, metadata);

    TimeSheetInfoList timeSheetInfo = createTimeSheetInfo();
    // extend report tag: add tasks and effort structure

    if (m_includeTaskList) {   // tasks
        QDomElement tasks = document.createElement(QStringLiteral("tasks"));
        report.appendChild(tasks);
        Q_FOREACH (const TimeSheetInfo &info, timeSheetInfo) {
            if (info.taskId == 0)   // the root task
                continue;
            const Task &modelTask = m_dataModel->getTask(info.taskId);
            tasks.appendChild(modelTask.toXml(document));
        }
    }
    {   // effort
        // make effort element:
        QDomElement effort = document.createElement(QStringLiteral("effort"));
        report.appendChild(effort);

        // aggregate (group by task and day):
        typedef QPair<TaskId, QDate> Key;
        QMap< Key, Event> events;
        Q_FOREACH (const Event &event, m_events) {
            TimeSheetInfoList::iterator it;
            for (it = timeSheetInfo.begin(); it != timeSheetInfo.end(); ++it)
                if ((*it).taskId == event.taskId()) break;
            if (it == timeSheetInfo.end())
                continue;
            Key key(event.taskId(), event.startDateTime().date());
            if (events.contains(key)) {
                // add to previous events:
                const Event &oldEvent = events[key];
                const int seconds = oldEvent.duration() + event.duration();
                const QDateTime start = oldEvent.startDateTime();
                const QDateTime end(start.addSecs(seconds));
                Q_ASSERT(start.secsTo(end) == seconds);
                Event newEvent(oldEvent);
                newEvent.setStartDateTime(start);
                newEvent.setEndDateTime(end);
                Q_ASSERT(newEvent.duration() == seconds);
                QString comment = oldEvent.comment();
                if (!event.comment().isEmpty()) {
                    if (!comment.isEmpty())     // make separator
                        comment += QLatin1String(" / ");
                    comment += event.comment();
                    newEvent.setComment(comment);
                }
                events[key] = newEvent;
            } else {
                // add this event:
                events[key] = event;
                events[key].setId(-events[key].id());   // "synthetic" :-)
                // move to start at midnight in UTC (for privacy reasons)
                // never, never, never use setTime() here, it breaks on DST changes! (twice a year)
                QDateTime start(event.startDateTime().date(), QTime(0, 0, 0, 0), Qt::UTC);
                QDateTime end(start.addSecs(event.duration()));
                events[key].setStartDateTime(start);
                events[key].setEndDateTime(end);
                Q_ASSERT(events[key].duration() == event.duration());
                Q_ASSERT(start.time() == QTime(0, 0, 0, 0));
            }
        }
        // create elements:
        Q_FOREACH (const Event &event, events)
            effort.appendChild(event.toXml(document));
    }

    return document.toByteArray(4);
}
