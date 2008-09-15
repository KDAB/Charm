#ifndef CHARM_MAINWINDOW_H
#define CHARM_MAINWINDOW_H

#include <QLabel>
#include <QMenu>
#include <QList>
#include <QAction>
#include <QActionGroup>
#include <QMainWindow>
#include <QSystemTrayIcon>

#include <Core/CommandEmitterInterface.h>
#include <Core/State.h>
#include <Core/ViewInterface.h>

#include "EventView.h"
#include "TasksView.h"
#include "ViewModeInterface.h"
#include "Reports/ReportDialog.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow,
                   public ViewInterface,
                   public CommandEmitterInterface
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

    void stateChanged( State previous );
    QAction* actionQuit();

public slots:
    void slotQuit();
    void slotShowHideView();
    void slotEditPreferences( bool ); // show prefs dialog
    void slotConfigurationChanged();
    void slotSelectViewMode( QAction* );
    void slotToggleView();
    void slotExportToXml();
    void slotImportFromXml();

    // CommandEmitterInterface:
    void commitCommand( CharmCommand* );
    void sendCommand( CharmCommand* );

    void restore();

private slots:
    void slotAboutDialog();
    void slotReportDialog();
    void slotUpdateTotal();
    void slotToggleStatusbar( bool );

protected:
    void showEvent( QShowEvent* );
    void hideEvent( QHideEvent* );

signals:
    void emitCommand( CharmCommand* ); // CommandEmitterInterface
    void visibilityChanged( bool ); // reimpl from ViewInterface
    void saveConfiguration();
    void quit();

private:
    void saveGuiState();
    void restoreGuiState();

    QList<ViewModeInterface*> m_modes;

    Ui::MainWindow* m_ui;
    QActionGroup m_viewActionsGroup;
    QAction m_actionQuit;
    QAction m_actionAboutDialog;
    QAction m_actionPreferences;
    QAction m_actionEventView;
    QAction m_actionTasksView;
    QAction m_actionToggleView;
    QAction m_actionShowStatusbar;
    QAction m_actionExportToXml;
    QAction m_actionImportFromXml;
    View m_tasksView;
    EventView m_eventView;
    QAction m_actionReporting;
    ReportDialog m_reportDialog;
    QLabel m_statusBarLabelWeekTotal;
    QLabel m_statusBarLabelDayTotal;
    bool m_statusBarWidgetsAdded;
};

#endif
