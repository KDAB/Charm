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
class QToolBar;
class QToolButton;
class QButton;
class QTreeView;

class TasksView : public QWidget,
             public ViewModeInterface,
             public CommandEmitterInterface
{
    Q_OBJECT

public:
    explicit TasksView ( QToolBar* toolBar, QWidget* parent = 0 );
    ~TasksView();

    // implement ViewModeInterface:
    void stateChanged( State previous );
    void configurationChanged();
    void setModel( ModelConnector* );

    void populateEditMenu( QMenu* );

public Q_SLOTS:
    /* reimpl */ void commitCommand( CharmCommand* );

signals:
    // FIXME connect to MainWindow
    void saveConfiguration();
    void emitCommand( CharmCommand* );
    void emitCommandRollback( CharmCommand* );

private slots:
    void actionNewTask();
    void actionNewSubTask();
    void actionEditTask();
    void actionDeleteTask();
    void slotFiltertextChanged( const QString& filtertext );
    void taskPrefilteringChanged();
    void slotContextMenuRequested( const QPoint& );

    void slotEventActivated( EventId );
    void slotEventDeactivated( EventId );
    // this method is called everytime the UI actions need update, for
    // example when the current index changes:
    void configureUi();
    void restoreGuiState();

private:
    void saveGuiState();

    // helper to retrieve selected task:
    Task selectedTask();
    void addTaskHelper( const Task& parent );

    TasksViewDelegate* m_delegate;
    QAction m_actionNewTask;
    QAction m_actionNewSubTask;
    QAction m_actionEditTask;
    QAction m_actionDeleteTask;
    QAction m_actionExpandTree;
    QAction m_actionCollapseTree;
    StatusBarWidget* m_statusBarWidget;
    QButton* m_showCurrentOnly;
    QButton* m_showSubscribedOnly;
    QTreeView* m_treeView;
};

#endif
