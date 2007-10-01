#ifndef CHARM_VIEW_H
#define CHARM_VIEW_H

#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QSystemTrayIcon>

#include "ViewInterface.h"
#include "EventEditor.h"
#include "Reports/ReportDialog.h"
#include "Commands/CommandEmitterInterface.h"

class QItemSelection;
class ChattyItemDelegate;
class StatusBarWidget;

namespace Ui {
    class View;
};

class View : public QMainWindow,
             public ViewInterface,
             public CommandEmitterInterface
{
    Q_OBJECT

public:
    explicit View ( QWidget* parent = 0 );
    ~View();

    void stateChanged( State previous );

    void closeEvent( QCloseEvent* );
    void showEvent( QShowEvent* );
    void hideEvent( QHideEvent* );

public slots:
    // reimpl
    void commitCommand( CharmCommand* );
    void restore();
    void sendCommand( CharmCommand* );

    // pure view slots:
    void slotItemDoubleClicked( const QModelIndex& index );
    void slotTrayIconActivated( QSystemTrayIcon::ActivationReason );
    void slotShowHideView();
    void slotStopAllTasks();

signals:
    void emitCommand( CharmCommand* );
    void saveConfiguration();
    void quit();

private slots:
    void actionSelectedEventStarted( bool );
    void actionSelectedEventEnded( bool );
    void actionNewTask( bool );
//     void viewSelectionChanged( const QItemSelection & selected,
//                                const QItemSelection & deselected );
    void viewCurrentChanged( const QModelIndex&, const QModelIndex& );
    void slotFiltertextChanged( const QString& filtertext );
    void subscribedOnlyModeChanged( int );
    void slotContextMenuRequested( const QPoint& );
    void slotEditPreferences( bool ); // show prefs dialog

    void slotEventActivated( EventId );
    void slotEventDeactivated( EventId );
    void slotQuit();
    // this method is called everytime the UI actions need update, for
    // example when the current index changes:
    void slotConfigureUi();
    void slotAboutDialog();

private:
    // this method is called when a permanent UI setting is changed:
    void configurePermanentUiSettings();
    // helper to retrieve selected task:
    Task selectedTask();
    void configureUi( const QModelIndex& current );

    void saveGuiState();
    void restoreGuiState();

    Ui::View* m_ui;
    ChattyItemDelegate* m_delegate;
    QAction m_actionEventStarted;
    QAction m_actionEventEnded;
    QAction m_actionSelectedEventStarted;
    QAction m_actionSelectedEventEnded;
    QAction m_actionNewTask;
    QAction m_actionNewSubTask;
    QAction m_actionDeleteTask;
    QAction m_actionQuit;
    QAction m_actionPreferences;
    QAction m_actionReporting;
    QAction m_actionEventEditor;
    QAction m_actionAboutDialog;
    QAction m_actionShowHideView;
    QAction m_actionStopAllTasks;
    EventEditor m_eventEditor;
    ReportDialog m_reportDialog;
    QSystemTrayIcon m_trayIcon;
    QMenu m_systrayContextMenu;
    StatusBarWidget* m_statusBarWidget;
};

#endif
