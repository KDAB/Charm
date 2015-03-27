/*
  MonthlyTimesheetConfigurationDialog.h

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

#ifndef MONTHLYTIMESHEETCONFIGURATIONDIALOG_H
#define MONTHLYTIMESHEETCONFIGURATIONDIALOG_H

#include <Core/Task.h>
#include <Core/TimeSpans.h>

#include "ReportConfigurationDialog.h"

#include <QScopedPointer>

namespace Ui { class MonthlyTimesheetConfigurationDialog; }

class MonthlyTimesheetConfigurationDialog : public ReportConfigurationDialog
{
    Q_OBJECT

public:
    explicit MonthlyTimesheetConfigurationDialog( QWidget* parent );
    virtual ~MonthlyTimesheetConfigurationDialog();

    void showReportPreviewDialog( QWidget* parent ) override;
    void showEvent( QShowEvent* ) override;
    void setDefaultMonth( int yearOfMonth, int month );

public Q_SLOTS:
    void accept() override;

private slots:
    void slotCheckboxSubtasksOnlyChecked( bool );
    void slotStandardTimeSpansChanged();
    void slotMonthComboItemSelected( int );
    void slotSelectTask();

private:
    QScopedPointer<Ui::MonthlyTimesheetConfigurationDialog> m_ui;
    QList<NamedTimeSpan> m_monthInfo;
    TaskId m_rootTask;
};

#endif
