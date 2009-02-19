#ifndef TASKSWINDOW_H
#define TASKSWINDOW_H

#include "CharmWindow.h"
#include "Reports/ReportDialog.h"

class TasksView; // FIXME rename to TasksView

class TasksWindow : public CharmWindow
{
    Q_OBJECT

public:
    explicit TasksWindow( QWidget* parent = 0 );
    ~TasksWindow();

    /* reimpl */ void stateChanged( State previous );

    // restore the view
    /* reimpl */ void restore();

public slots:
    // slots migrated from the old main window:
    void slotEditPreferences( bool ); // show prefs dialog
    void slotAboutDialog();
    void slotReportDialog();
    void slotExportToXml();
    void slotImportFromXml();
    void slotImportTasks();
    void maybeIdle();

signals:
    /* reimpl */ void emitCommand( CharmCommand* );
    /* reimpl */ void saveConfiguration();
    /* reimpl */ void quit();


private:
    TasksView* m_tasksView;
    // the tasks window is also the "default view" that is made
    // visible when dialogs are shown:
    ReportDialog m_reportDialog;

};

#endif
