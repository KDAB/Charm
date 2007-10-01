#ifndef TASKMODELADAPTER_H
#define TASKMODELADAPTER_H

#include <QAbstractItemModel>

#include "TaskModelInterface.h"
#include "CharmDataModel.h"
#include "CharmDataModelAdapterInterface.h"
#include "Commands/CommandEmitterInterface.h"

enum ViewColumns {
    Column_TaskId,
    Column_TaskName,
    Column_TaskSubscriptions,
    Column_TaskSessionTime,
    Column_TaskComment,
    Column_TaskColumnCount
};

typedef ViewColumns ViewColumn;

/** TaskModelAdapter adapts the CharmDataModel to be used in the task view.

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
    QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    Qt::ItemFlags flags( const QModelIndex & index ) const;
    bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

    // reimplement CharmDataModelAdapterInterface:
    void resetTasks();
    void taskAboutToBeAdded( TaskId parent, int pos );
    void taskAdded( TaskId id );
    void taskModified( TaskId id );
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
