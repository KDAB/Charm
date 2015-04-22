/*
  WeeklyTimesheet.h

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

#ifndef WEEKLYTIMESHEET3_H
#define WEEKLYTIMESHEET3_H

#include <Core/Task.h>
#include <Core/TimeSpans.h>

#include "Timesheet.h"
#include "ReportConfigurationDialog.h"

#include <QScopedPointer>

namespace Ui {
    class WeeklyTimesheetConfigurationDialog;
}

class HttpJob;
class QUrl;

typedef QHash<int, QVector<int> > WeeksByYear;
///Set the timesheet for the @param week of the @param year as having been uploaded
void addUploadedTimesheet(int year, int week);
///Get all missing timesheets
WeeksByYear missingTimeSheets();

class WeeklyTimesheetConfigurationDialog : public ReportConfigurationDialog
{
    Q_OBJECT

public:
    explicit WeeklyTimesheetConfigurationDialog( QWidget* parent );
    ~WeeklyTimesheetConfigurationDialog();

    void showReportPreviewDialog( QWidget* parent ) override;
    void showEvent( QShowEvent* ) override;
    void setDefaultWeek( int yearOfWeek, int week );

public Q_SLOTS:
    void accept() override;

private slots:
    void slotCheckboxSubtasksOnlyChecked( bool );
    void slotStandardTimeSpansChanged();
    void slotWeekComboItemSelected( int );
    void slotSelectTask();

private:
    QScopedPointer<Ui::WeeklyTimesheetConfigurationDialog> m_ui;
    QList<NamedTimeSpan> m_weekInfo;
    TaskId m_rootTask;
};

class WeeklyTimeSheetReport : public TimeSheetReport
{
    Q_OBJECT

public:
    explicit WeeklyTimeSheetReport( QWidget* parent = nullptr );
    virtual ~WeeklyTimeSheetReport();

    void setReportProperties( const QDate& start,
                              const QDate& end,
                              TaskId rootTask,
                              bool activeTasksOnly ) override;

private slots:
    void slotUploadTimesheet();
    void slotTimesheetUploaded( HttpJob* );
    void slotLinkClicked( const QUrl& which );

private:
    QString suggestedFileName() const override;
    void update() override;
    QByteArray saveToXml() override;
    QByteArray saveToText() override;

private:
    // properties of the report:
    int m_weekNumber;
    int m_yearOfWeek;
};

#endif
