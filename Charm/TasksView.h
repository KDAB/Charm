#ifndef CHARM_VIEW_H
#define CHARM_VIEW_H

#include <QWidget>
#include <QAction>

#include <Core/Event.h>
#include <Core/State.h>
#include <Core/CommandEmitterInterface.h>

#include "ViewModeInterface.h"

class QMenu;
class QItemSelection;
class QModelIndex;
class TasksViewDelegate;
class StatusBarWidget;

namespace Ui {
    class TasksView;
}

class TasksView : public QWidget,
             public ViewModeInterface,
             public CommandEmitterInterface
{
    Q_OBJECT

public:
    explicit TasksView ( QWidget* parent = 0 );
    ~TasksView();

    void closeEvent( QCloseEvent* );
    void showEvent( QShowEvent* );
    // void hideEvent( QHideEvent* );

    // implement ViewModeInterface:
    void saveGuiState();
    void restoreGuiState();
    void stateChanged( State previous );
    void configurationChanged();
    void setModel( ModelConnector* );

    static QFont configuredFont();
    void populateEditMenu( QMenu* );

public slots:
    /* reimpl */ void commitCommand( CharmCommand* );

private slots:
    void slotItemDoubleClicked( const QModelIndex& index );

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
    void taskPrefilteringChanged( int );
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

    Ui::TasksView* m_ui;
    TasksViewDelegate* m_delegate;
    QAction m_actionEventStarted;
    QAction m_actionEventEnded;
    QAction m_actionSelectedEventStarted;
    QAction m_actionSelectedEventEnded;
    QAction m_actionNewTask;
    QAction m_actionNewSubTask;
    QAction m_actionEditTask;
    QAction m_actionDeleteTask;
    StatusBarWidget* m_statusBarWidget;
};

#endif
