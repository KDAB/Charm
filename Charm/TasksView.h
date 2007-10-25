#ifndef CHARM_VIEW_H
#define CHARM_VIEW_H

#include <QWidget>
#include <QAction>
#include <QMenu>

#include <Core/Event.h>
#include <Core/State.h>
#include <Core/CommandEmitterInterface.h>

#include "ViewModeInterface.h"

class QItemSelection;
class QModelIndex;
class ChattyItemDelegate;
class StatusBarWidget;

namespace Ui {
    class View;
};

class View : public QWidget,
             public ViewModeInterface,
             public CommandEmitterInterface
{
    Q_OBJECT

public:
    explicit View ( QWidget* parent = 0 );
    ~View();

    void closeEvent( QCloseEvent* );
    void showEvent( QShowEvent* );
    // void hideEvent( QHideEvent* );

    // implement ViewModeInterface:
    void saveGuiState();
    void restoreGuiState();
    void stateChanged( State previous );
    void configurationChanged();
    void setModel( ModelConnector* );

    QAction* actionStopAllTasks();

public slots:
    // reimpl
    void commitCommand( CharmCommand* );

    // pure view slots:
    void slotItemDoubleClicked( const QModelIndex& index );
    void slotStopAllTasks();

signals:
    // FIXME connect to MainWindow
    void saveConfiguration();
    void emitCommand( CharmCommand* );

private slots:
    void actionSelectedEventStarted( bool );
    void actionSelectedEventEnded( bool );
    // void actionNewTask( bool );
    void viewCurrentChanged( const QModelIndex&, const QModelIndex& );
    void slotFiltertextChanged( const QString& filtertext );
    void subscribedOnlyModeChanged( int );
    void slotContextMenuRequested( const QPoint& );

    void slotEventActivated( EventId );
    void slotEventDeactivated( EventId );
    // this method is called everytime the UI actions need update, for
    // example when the current index changes:
    void slotConfigureUi();

private:
    // helper to retrieve selected task:
    Task selectedTask();
    void configureUi( const QModelIndex& current );

    Ui::View* m_ui;
    ChattyItemDelegate* m_delegate;
    QAction m_actionEventStarted;
    QAction m_actionEventEnded;
    QAction m_actionSelectedEventStarted;
    QAction m_actionSelectedEventEnded;
    QAction m_actionNewTask;
    QAction m_actionNewSubTask;
    QAction m_actionDeleteTask;
    QAction m_actionStopAllTasks;
    StatusBarWidget* m_statusBarWidget;
};

#endif
