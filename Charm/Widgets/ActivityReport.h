/*
  ActivityReport.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2014-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

#include <QScopedPointer>

namespace Ui {
    class ActivityReportConfigurationDialog;
}

class QUrl;

class ActivityReportConfigurationDialog : public ReportConfigurationDialog
{
    Q_OBJECT

public:
    explicit ActivityReportConfigurationDialog( QWidget* parent );
    ~ActivityReportConfigurationDialog();

    void showReportPreviewDialog() override;

public Q_SLOTS:
    void accept() override;

private slots:
    void slotDelayedInitialization();
    void slotStandardTimeSpansChanged();
    void slotTimeSpanSelected( int );
    void slotSelectTask();
    void slotExcludeTask();
    void slotRemoveExcludedTask();
    void slotRemoveIncludeTask();

private:
    bool selectTask(TaskId& task);

    QScopedPointer<Ui::ActivityReportConfigurationDialog> m_ui;
    QList<NamedTimeSpan> m_timespans;
    QSet<TaskId> m_rootTasks;
    QSet<TaskId> m_rootExcludeTasks;
};

class ActivityReport : public ReportPreviewWindow
{
    Q_OBJECT

public:
    explicit ActivityReport( QWidget* parent = nullptr );
    ~ActivityReport();

    void setReportProperties(const QDate& start, const QDate& end,
        QSet<TaskId> rootTasks, QSet<TaskId> rootExcludeTasks );
    void timeSpanSelection( NamedTimeSpan timeSpanSelection );

private slots:
    void slotLinkClicked( const QUrl& which );

private:
    void slotUpdate() override;

private:
    QDate m_start;
    QDate m_end;
    QSet<TaskId> m_rootTasks;
    QSet<TaskId> m_rootExcludeTasks;
    NamedTimeSpan m_timeSpanSelection;
};

#endif
