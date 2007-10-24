#ifndef CHARM_MAINWINDOW_H
#define CHARM_MAINWINDOW_H

#include <QMenu>
#include <QList>
#include <QAction>
#include <QActionGroup>
#include <QMainWindow>
#include <QSystemTrayIcon>

#include <Core/CommandEmitterInterface.h>
#include <Core/State.h>
#include <Core/ViewInterface.h>

#include "EventEditor.h"
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

public slots:
    void slotQuit();
    void slotShowHideView();
    void slotTrayIconActivated( QSystemTrayIcon::ActivationReason );
    void slotEditPreferences( bool ); // show prefs dialog
    void slotConfigurationChanged();
    void slotSelectViewMode( QAction* );
    void slotCurrentBackendStatusChanged( const QString& text );

    // CommandEmitterInterface:
    void commitCommand( CharmCommand* );
    void sendCommand( CharmCommand* );

    void restore();
    void slotAboutDialog();

protected:
    void showEvent( QShowEvent* );
    void hideEvent( QHideEvent* );

signals:
    void emitCommand( CharmCommand* ); // CommandEmitterInterface
    void saveConfiguration();
    void quit();

private:
    void saveGuiState();
    void restoreGuiState();

    QList<ViewModeInterface*> m_modes;

    Ui::MainWindow* m_ui;
    QSystemTrayIcon m_trayIcon;
    QMenu m_systrayContextMenu;
    QActionGroup m_viewActionsGroup;
    QAction m_actionShowHideView;
    QAction m_actionQuit;
    QAction m_actionAboutDialog;
    QAction m_actionPreferences;
    QAction m_actionEventEditor;
    QAction m_actionTasksView;
    View m_tasksView;
    EventEditor m_eventEditor;
    QAction m_actionReporting;
    ReportDialog m_reportDialog;
};

#endif
