/*
  ActivityReport.h

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

#ifndef ACTIVITYREPORT_H
#define ACTIVITYREPORT_H

#include <Core/Task.h>
#include <Core/TimeSpans.h>

#include "ReportConfigurationDialog.h"
#include "ReportPreviewWindow.h"
#include "ViewHelpers.h"

#include <QScopedPointer>

namespace Ui {
class ActivityReportConfigurationDialog;
}

class QUrl;
class QListWidgetItem;
class QListWidget;

class ActivityReportConfigurationDialog : public ReportConfigurationDialog
{
    Q_OBJECT

public:
    struct Properties {
        NamedTimeSpan timeSpanSelection;
        QDate start;
        QDate end;
        QSet<TaskId> rootTasks;
        QSet<TaskId> rootExcludeTasks;
        bool showFullDescription = false;
        bool groupByTaskId = false;
        bool groupByTaskIdAndComments = false;
    };

    explicit ActivityReportConfigurationDialog(QWidget *parent);
    ~ActivityReportConfigurationDialog() override;

    void showReportPreviewDialog() override;

public Q_SLOTS:
    void accept() override;
    void reject() override;

private Q_SLOTS:
    void slotDelayedInitialization();
    void slotStandardTimeSpansChanged();
    void slotTimeSpanSelected(int);
    void slotSelectTask();
    void slotExcludeTask();
    void slotRemoveExcludedTask();
    void slotRemoveIncludeTask();
    void slotGroupTasks(bool checked);
    void slotGroupTasksComments(bool checked);

private:
    bool selectTask(TaskId &task);
    QListWidgetItem *addListItem(TaskId id, QListWidget *list) const;

    QScopedPointer<Ui::ActivityReportConfigurationDialog> m_ui;
    QList<NamedTimeSpan> m_timespans;
    ActivityReportConfigurationDialog::Properties m_properties;
};

class ActivityReport : public ReportPreviewWindow
{
    Q_OBJECT

public:
    explicit ActivityReport(QWidget *parent = nullptr);
    ~ActivityReport() override;

    void setReportProperties(const ActivityReportConfigurationDialog::Properties &properties);

private Q_SLOTS:
    void updateRange(int direction);

private:
    void slotUpdate() override;

private:
    ActivityReportConfigurationDialog::Properties m_properties;
};

#endif
