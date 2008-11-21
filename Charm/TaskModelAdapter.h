#ifndef TASKMODELADAPTER_H
#define TASKMODELADAPTER_H

#include <QAbstractItemModel>

#include "Core/TaskModelInterface.h"
#include "Core/CharmDataModel.h"
#include "Core/CharmDataModelAdapterInterface.h"
#include "Core/CommandEmitterInterface.h"

enum ViewColumns {
    Column_TaskId, // FIXME rename
    Column_TaskColumnCount
};

enum TasksViewRoles {
    TasksViewRole_Name = 0x1045F132,
    TasksViewRole_RunningTime,
    TasksViewRole_Comment
};

typedef ViewColumns ViewColumn;

/** TaskModelAdapter adapts the CharmDataModel to be used in the task view
    (in main view and "select task" dialog).

    It is a QAbstractItemModel, and stores the TaskTreeItem pointer of
    the respective address in the model indexes internal pointer.
*/
class TaskModelAdapter :  public QAbstractItemModel,
                          public TaskModelInterface,
                          public CommandEmitterInterface,
                          public CharmDataModelAdapterInterface
{
    Q_OBJECT

public:
    explicit TaskModelAdapter( CharmDataModel* parent );
    ~TaskModelAdapter();

    // reimplement QAbstractItemModel:
    int columnCount( const QModelIndex& parent = QModelIndex() ) const;
    int rowCount( const QModelIndex& parent = QModelIndex() ) const;
    QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const;
    QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    QModelIndex parent( const QModelIndex & index ) const;
    // QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    Qt::ItemFlags flags( const QModelIndex & index ) const;
    bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

    // reimplement CharmDataModelAdapterInterface:
    void resetTasks();
    void taskAboutToBeAdded( TaskId parent, int pos );
    void taskAdded( TaskId id );
    void taskModified( TaskId id );
    void taskParentChanged( TaskId task, TaskId oldParent, TaskId newParent );
    void taskAboutToBeDeleted( TaskId );
    void taskDeleted( TaskId id );

    void resetEvents() {}
    void eventAboutToBeAdded( EventId id ) {}
    void eventAdded( EventId );
    void eventModified( EventId, Event );
    void eventAboutToBeDeleted( EventId id ) {}
    void eventDeleted( EventId );

    void eventActivated( EventId id );
    void eventDeactivated( EventId id );

    // reimplement TaskModelInterface:
    Task taskForIndex( const QModelIndex& ) const;
    QModelIndex indexForTaskId( TaskId ) const;
    bool taskIsActive( const Task& task ) const;
    bool taskHasChildren( const Task& task ) const;
    bool taskIdExists( TaskId taskId ) const;

    // reimplement CommandEmitterInterface:
    void commitCommand( CharmCommand* );

signals:
    void eventActivationNotice( EventId id );
    void eventDeactivationNotice( EventId id );

private:
    const TaskTreeItem* itemFor ( const QModelIndex& ) const;
    QModelIndex indexForTaskTreeItem( const TaskTreeItem& item, int column = 0 ) const;

    CharmDataModel* m_dataModel;
};

#endif
