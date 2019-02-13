/*
  CharmConstants.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2007-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>
  Author: Frank Osterfeld <frank.osterfeld@kdab.com>
  Author: Olivier JG <olivier.de.gaalon@kdab.com>

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

#include "CharmConstants.h"
#include "CharmDataModel.h"
#include "Controller.h"

#include <QObject>
#include <QTextStream>

const QString MetaKey_EventsInLeafsOnly = QStringLiteral("EventsInLeafsOnly");
const QString MetaKey_OneEventAtATime = QStringLiteral("OneEventAtATime");
const QString MetaKey_MainWindowGeometry = QStringLiteral("MainWindowGeometry");
const QString MetaKey_MainWindowVisible = QStringLiteral("MainWindowVisible");
const QString MetaKey_MainWindowGUIStateSelectedTask = QStringLiteral(
    "MainWindowGUIStateSelectedTask");
const QString MetaKey_MainWindowGUIStateExpandedTasks = QStringLiteral(
    "MainWindowGUIStateExpandedTasks");
const QString MetaKey_MainWindowGUIStateShowExpiredTasks = QStringLiteral(
    "MainWindowGUIStateShowExpiredTasks");
const QString MetaKey_MainWindowGUIStateShowCurrentTasks = QStringLiteral(
    "MainWindowGUIStateShowCurrentTasks");
const QString MetaKey_TimeTrackerGeometry = QStringLiteral("TimeTrackerGeometry");
const QString MetaKey_TimeTrackerVisible = QStringLiteral("TimeTrackerVisible");
const QString MetaKey_EventEditorGeometry = QStringLiteral("EventEditorGeometry");
const QString MetaKey_TaskEditorGeometry = QStringLiteral("TaskEditorGeometry");
const QString MetaKey_ReportsRecentSavePath = QStringLiteral("ReportsRecentSavePath");
const QString MetaKey_ExportToXmlRecentSavePath = QStringLiteral("ExportToXmlSavePath");
const QString MetaKey_TimesheetSubscribedOnly = QStringLiteral("TimesheetSubscribedOnly");
const QString MetaKey_TimesheetActiveOnly = QStringLiteral("TimesheetActiveOnly");
const QString MetaKey_TimesheetRootTask = QStringLiteral("TimesheetRootTask");
const QString MetaKey_LastEventEditorDateTime = QStringLiteral("LastEventEditorDateTime");
const QString MetaKey_Key_InstallationId = QStringLiteral("InstallationId");
const QString MetaKey_Key_UserName = QStringLiteral("UserName");
const QString MetaKey_Key_UserId = QStringLiteral("UserId");
const QString MetaKey_Key_LocalStorageDatabase = QStringLiteral("LocalStorageDatabase");
const QString MetaKey_Key_LocalStorageType = QStringLiteral("LocalStorageType");
const QString MetaKey_Key_SubscribedTasksOnly = QStringLiteral("SubscribedTasksOnly");
const QString MetaKey_Key_TimeTrackerFontSize = QStringLiteral("TimeTrackerFontSize");
const QString MetaKey_Key_24hEditing = QStringLiteral("Key24hEditing");
const QString MetaKey_Key_DurationFormat = QStringLiteral("DurationFormat");
const QString MetaKey_Key_IdleDetection = QStringLiteral("IdleDetection");
const QString MetaKey_Key_WarnUnuploadedTimesheets = QStringLiteral("WarnUnuploadedTimesheets");
const QString MetaKey_Key_RequestEventComment = QStringLiteral("RequestEventComment");
const QString MetaKey_Key_ToolButtonStyle = QStringLiteral("ToolButtonStyle");
const QString MetaKey_Key_ShowStatusBar = QStringLiteral("ShowStatusBar");
const QString MetaKey_Key_EnableCommandInterface = QStringLiteral("EnableCommandInterface");
const QString MetaKey_Key_NumberOfTaskSelectorEntries = QStringLiteral("NumberOfTaskSelectorEntries");

const QString TrueString(QStringLiteral("true"));
const QString FalseString(QStringLiteral("false"));

const QString &stringForBool(bool val)
{
    return val ? TrueString : FalseString;
}

void connectControllerAndModel(Controller *controller, CharmDataModel *model)
{
    QObject::connect(controller, SIGNAL(eventAdded(Event)),
                     model, SLOT(addEvent(Event)));
    QObject::connect(controller, SIGNAL(eventModified(Event)),
                     model, SLOT(modifyEvent(Event)));
    QObject::connect(controller, SIGNAL(eventDeleted(Event)),
                     model, SLOT(deleteEvent(Event)));
    QObject::connect(controller, SIGNAL(allEvents(EventList)),
                     model, SLOT(setAllEvents(EventList)));
    QObject::connect(controller, SIGNAL(definedTasks(TaskList)),
                     model, SLOT(setAllTasks(TaskList)));
    QObject::connect(controller, SIGNAL(taskAdded(Task)),
                     model, SLOT(addTask(Task)));
    QObject::connect(controller, SIGNAL(taskUpdated(Task)),
                     model, SLOT(modifyTask(Task)));
    QObject::connect(controller, SIGNAL(taskDeleted(Task)),
                     model, SLOT(deleteTask(Task)));
}

static QString formatDecimal(double d)
{
    const QString s = QLocale::system().toString(d, 'f', 2);
    if (d > -10 && d < 10) { //hack to get the hours always have two decimals: e.g. 00.50 instead of 0.50
        return QStringLiteral("0") + s;
    } else {
        return s;
    }
}

QString hoursAndMinutes(int duration)
{
    if (duration == 0) {
        if (CONFIGURATION.durationFormat == Configuration::Minutes) {
            return QObject::tr("00:00");
        } else {
            return formatDecimal(0.0);
        }
    }
    int minutes = duration / 60;
    int hours = minutes / 60;
    minutes = minutes % 60;

    if (CONFIGURATION.durationFormat == Configuration::Minutes) {
        QString text;
        QTextStream stream(&text);
        stream << qSetFieldWidth(2) << qSetPadChar(QLatin1Char('0'))
               << hours << qSetFieldWidth(0) << ":" << qSetFieldWidth(2) << minutes;
        return text;
    } else { //Decimal
        return formatDecimal(hours + minutes / 60.0);
    }
}
