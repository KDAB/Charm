#ifndef TimeTrackingWindow_H
#define TimeTrackingWindow_H

#include "Core/ViewInterface.h"
#include "Core/CharmDataModelAdapterInterface.h"

#include "CharmWindow.h"
#include "TimeTrackingView/WeeklySummary.h"

class HttpJob;
class CharmCommand;
class TimeTrackingView;
class ReportConfigurationDialog;
class WeeklyTimesheetConfigurationDialog;
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
    void slotExportToXml();
    void slotImportFromXml();
    void slotSyncTasks();
    void slotImportTasks();
    void slotExportTasks();
    void maybeIdle();
    void slotTasksDownloaded( HttpJob* );

protected:
    void importTasksFromFile(const QString &filename);

    /* reimpl */ void insertEditMenu();

private slots:
    void slotStartEvent( TaskId );
    void slotStopEvent();
    void slotSelectTasksToShow();
    void slotWeeklyTimesheetPreview( int result );
    void slotActivityReportPreview( int result );

signals:
    void emitCommand( CharmCommand* );
    void emitCommandRollback( CharmCommand* );

private:
    void showPreview( ReportConfigurationDialog*, int result );

    WeeklyTimesheetConfigurationDialog* m_weeklyTimesheetDialog;
    ActivityReportConfigurationDialog *m_activityReportDialog;
    TimeTrackingView* m_summaryWidget;
    QVector<WeeklySummary> m_summaries;
};

#endif
