/*
  Configuration.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2017-2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

#ifndef LOTSOFCAKE_CONFIGURATION_H
#define LOTSOFCAKE_CONFIGURATION_H

#include <QDate>

class QString;
class QUrl;

class TaskExport;

namespace Lotsofcake {

class Configuration
{
public:
    bool isConfigured() const;

    void importFromTaskExport(const TaskExport &exporter);

    QDate lastStagedTimesheetUpload() const;
    void setLastStagedTimesheetUpload(const QDate &date);

    QString username() const;
    QUrl timesheetUploadUrl() const;
    QUrl projectCodeDownloadUrl() const;
    QUrl restUrl() const;

private:
    mutable struct {
        bool set = false;
        QDate date;
    } m_lastStagedTimesheetUpload;
};

}

#endif
