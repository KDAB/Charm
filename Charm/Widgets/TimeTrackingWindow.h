/*
  TimeTrackingWindow.h

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

#ifndef TimeTrackingWindow_H
#define TimeTrackingWindow_H

#include <QTimer>

#include "Core/ViewInterface.h"
#include "Core/CharmDataModelAdapterInterface.h"

#include "CharmWindow.h"
#include "WeeklySummary.h"
#include "BillDialog.h"

class HttpJob;
class CharmCommand;
class TimeTrackingView;
class IdleDetector;
class ReportConfigurationDialog;
class WeeklyTimesheetConfigurationDialog;
class MonthlyTimesheetConfigurationDialog;
class ActivityReportConfigurationDialog;

class TimeTrackingWindow : public CharmWindow,
                         public CharmDataModelAdapterInterface
{
    Q_OBJECT
public:
    explicit TimeTrackingWindow( QWidget* parent = 0 );
    ~TimeTrackingWindow();

    // application:
    void stateChanged( State previous );
    void restore();
    void quit();

    bool event( QEvent* );
    void showEvent( QShowEvent* );
    QMenu* menu() const;
    // model adapter:
    void resetTasks();
    void taskAboutToBeAdded( TaskId parent, int pos );
    void taskAdded( TaskId id );
    void taskModified( TaskId id );
    void taskParentChanged( TaskId task, TaskId oldParent, TaskId newParent );
    void taskAboutToBeDeleted( TaskId );
    void taskDeleted( TaskId id );
    void resetEvents();
    void eventAboutToBeAdded( EventId id );
    void eventAdded( EventId id );
    void eventModified( EventId id, Event discardedEvent );
    void eventAboutToBeDeleted( EventId id );
    void eventDeleted( EventId id );
    void eventActivated( EventId id );
    void eventDeactivated( EventId id );

public slots:
    // slots migrated from the old main window:
    void slotEditPreferences( bool ); // show prefs dialog
    void slotAboutDialog();
    void slotEnterVacation();
    void slotActivityReport();
    void slotWeeklyTimesheetReport();
    void slotMonthlyTimesheetReport();
    void slotExportToXml();
    void slotImportFromXml();
    void slotSyncTasks();
    void slotImportTasks();
    void slotExportTasks();
    void maybeIdle( IdleDetector* idleDetector );
    void slotTasksDownloaded( HttpJob* );
    void slotUserInfoDownloaded( HttpJob* );

protected:

    /* reimpl */ void insertEditMenu();

private slots:
    void slotStartEvent( TaskId );
    void slotStopEvent();
    void slotSelectTasksToShow();
    void slotWeeklyTimesheetPreview( int result );
    void slotMonthlyTimesheetPreview( int result );
    void slotActivityReportPreview( int result );
    void slotCheckUploadedTimesheets();
    void slotBillGone( int result );

    void configurationChanged();

signals:
    void emitCommand( CharmCommand* );
    void emitCommandRollback( CharmCommand* );

private:
    void resetWeeklyTimesheetDialog();
    void resetMonthlyTimesheetDialog();
    void showPreview( ReportConfigurationDialog*, int result );
    //ugly but private:
    void importTasksFromDeviceOrFile( QIODevice* device, const QString& filename );
    void getUserInfo();

    WeeklyTimesheetConfigurationDialog* m_weeklyTimesheetDialog;
    MonthlyTimesheetConfigurationDialog* m_monthlyTimesheetDialog;
    ActivityReportConfigurationDialog *m_activityReportDialog;
    TimeTrackingView* m_summaryWidget;
    QVector<WeeklySummary> m_summaries;
    QTimer m_checkUploadedSheetsTimer;
    BillDialog *m_billDialog;
    QString m_user;
};

#endif
