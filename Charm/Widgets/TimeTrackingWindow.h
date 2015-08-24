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

#include "HttpClient/CheckForUpdatesJob.h"

#include "CharmWindow.h"
#include "WeeklySummary.h"
#include "BillDialog.h"

class HttpJob;
class CheckForUpdatesJob;
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
    explicit TimeTrackingWindow( QWidget* parent = nullptr );
    ~TimeTrackingWindow();

    enum VerboseMode {
        Verbose = 0,
        Silent
    };
    // application:
    void stateChanged( State previous ) override;
    void restore() override;
    void quit() override;

    bool event( QEvent* ) override;
    void showEvent( QShowEvent* ) override;
    QMenu* menu() const;
    // model adapter:
    void resetTasks() override;
    void taskAboutToBeAdded( TaskId parent, int pos ) override;
    void taskAdded( TaskId id ) override;
    void taskModified( TaskId id ) override;
    void taskParentChanged( TaskId task, TaskId oldParent, TaskId newParent ) override;
    void taskAboutToBeDeleted( TaskId ) override;
    void taskDeleted( TaskId id ) override;
    void resetEvents() override;
    void eventAboutToBeAdded( EventId id ) override;
    void eventAdded( EventId id ) override;
    void eventModified( EventId id, Event discardedEvent ) override;
    void eventAboutToBeDeleted( EventId id ) override;
    void eventDeleted( EventId id ) override;
    void eventActivated( EventId id ) override;
    void eventDeactivated( EventId id ) override;

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
    void slotSyncTasks( VerboseMode mode = Verbose );
    void slotImportTasks();
    void slotExportTasks();
    void maybeIdle( IdleDetector* idleDetector );
    void slotTasksDownloaded( HttpJob* );
    void slotUserInfoDownloaded( HttpJob* );
    void slotCheckForUpdatesManual();

protected:

    void insertEditMenu() override;

private slots:
    void slotStartEvent( TaskId );
    void slotStopEvent();
    void slotSelectTasksToShow();
    void slotWeeklyTimesheetPreview( int result );
    void slotMonthlyTimesheetPreview( int result );
    void slotActivityReportPreview( int result );
    void slotCheckUploadedTimesheets();
    void slotBillGone( int result );
    void slotCheckForUpdatesAutomatic();
    void slotCheckForUpdates( CheckForUpdatesJob::JobData );
    void slotSyncTasksAutomatic();

    void configurationChanged() override;

signals:
    void emitCommand( CharmCommand* );
    void emitCommandRollback( CharmCommand* );
    void showNotification( const QString& title, const QString& message );

private:
    void resetWeeklyTimesheetDialog();
    void resetMonthlyTimesheetDialog();
    void showPreview( ReportConfigurationDialog*, int result );
    //ugly but private:
    void importTasksFromDeviceOrFile( QIODevice* device, const QString& filename, bool verbose = true );
    void getUserInfo();
    void startCheckForUpdates( VerboseMode mode = Silent );
    void informUserAboutNewRelease( const QString& releaseVersion, const QUrl& link , const QString& releaseInfoLink );

    WeeklyTimesheetConfigurationDialog* m_weeklyTimesheetDialog;
    MonthlyTimesheetConfigurationDialog* m_monthlyTimesheetDialog;
    ActivityReportConfigurationDialog *m_activityReportDialog;
    TimeTrackingView* m_summaryWidget;
    QVector<WeeklySummary> m_summaries;
    QTimer m_checkUploadedSheetsTimer;
    QTimer m_checkCharmReleaseVersionTimer;
    QTimer m_updateTasksDefinitionsTimer;
    BillDialog *m_billDialog;
    CheckForUpdatesJob* m_checkForUpdatesJob;
    QString m_user;
};

#endif
