#include <QPixmap>
#include <QApplication>
#include <QPalette>

#include "Data.h"
#include "Core/CharmConstants.h"
#include "ViewHelpers.h"
#include <Core/Configuration.h>
#include <Core/CharmCommand.h>
#include "Commands/CommandModifyTask.h"
#include "Commands/CommandModifyEvent.h"
#include "TaskModelAdapter.h"

TaskModelAdapter::TaskModelAdapter( CharmDataModel* parent )
    : QAbstractItemModel()
    , m_dataModel( parent )
{
	m_dataModel->registerAdapter( this );
}

TaskModelAdapter::~TaskModelAdapter()
{
	m_dataModel->unregisterAdapter( this );
}

// reimplement QAbstractItemModel:
int TaskModelAdapter::columnCount( const QModelIndex& parent ) const
{
    return Column_TaskColumnCount;
}

int TaskModelAdapter::rowCount( const QModelIndex& parent ) const
{
    if ( parent.column() > 0 ) return 0;

    const TaskTreeItem* item = itemFor( parent );
    // every index has an item, the invalid index
    // has the root item
    Q_ASSERT( item );
    return item->childCount();
}

QVariant TaskModelAdapter::data( const QModelIndex& index, int role ) const
{
    if ( ! index.isValid() ) return QVariant();

    const TaskTreeItem* item = itemFor( index );
    const TaskId id = item->task().id();
    const Event& activeEvent = m_dataModel->activeEventFor( id );
    const bool isActive = activeEvent.isValid();
    const QApplication* application = static_cast<QApplication*>( QApplication::instance() );
    Q_ASSERT( application ); // we assume this code is executed in a GUI app

    // handle roles that are treated all the same, everywhere:
    switch( role ) {
    // problem: foreground role is never queried for
    case Qt::ForegroundRole:
    	if( item->task().isCurrentlyValid() ) {
            return application->palette().color( QPalette::Active, QPalette::Text );
    	} else {
    		return application->palette().color( QPalette::Disabled, QPalette::Text );
    	}
        break;
    case Qt::BackgroundRole:
    	if( item->task().isCurrentlyValid() ) {
            return QVariant();
    	} else {
    		QColor color( "crimson" );
    		color.setAlphaF( 0.25 );
    		return color;
    	}
        break;
    case Qt::DisplayRole:
    {
        // Return ID + name -- this is used for painting but also
        // for filtering in select task dialog.
        return DATAMODEL->taskIdAndNameString( item->task().id() );
    }
        break;
    case Qt::DecorationRole:
        if ( isActive ) {
            return Data::activePixmap();
        } else {
            return QVariant();
        }
        break;
    case Qt::CheckStateRole:
        if ( item->task().subscribed() ) {
            return Qt::Checked;
        } else {
            return Qt::Unchecked;
        }
        break;
    case TasksViewRole_Name: // now unused
        return item->task().name();
    case TasksViewRole_RunningTime:
        return hoursAndMinutes( activeEvent.duration() );
    case TasksViewRole_TaskId:
        return id;
    case Qt::EditRole: // we edit the comment
    case TasksViewRole_Comment:
        return activeEvent.comment();
    default:
        return QVariant();
    }
}

QModelIndex TaskModelAdapter::index( int row, int column, const QModelIndex & parent ) const
{   // sanity check:
    if ( row < 0 || column < 0 || column >= Column_TaskColumnCount )
        return QModelIndex();

    const TaskTreeItem* parentItem = itemFor( parent );
    Q_ASSERT( parentItem!= 0 );

    // more sanity checks:
    if ( row >= parentItem->childCount() )
        return QModelIndex();

    const TaskTreeItem& item = parentItem->child( row );

    if ( item.isValid() ) {
        return indexForTaskTreeItem( item, column );
    } else {
        return QModelIndex();
    }
}

QModelIndex TaskModelAdapter::parent( const QModelIndex & index ) const
{
    if ( ! index.isValid() ) return QModelIndex();

    const TaskTreeItem* item = itemFor( index );
    const TaskTreeItem& parent = m_dataModel->parentItem( item->task() );
    if ( !parent.isValid() )
        return QModelIndex(); // top level item

    return indexForTaskTreeItem( parent, 0 );
}

Qt::ItemFlags TaskModelAdapter::flags( const QModelIndex & index ) const
{
    Qt::ItemFlags flags = 0;

    if ( index.isValid() ) {
        const TaskTreeItem* item = itemFor( index );
        flags = Qt::ItemIsUserCheckable|Qt::ItemIsSelectable|Qt::ItemIsEnabled;
        const bool isCurrent = item->task().isCurrentlyValid();
        if ( isCurrent ) {
            const TaskId id = item->task().id();
            const Event& activeEvent = m_dataModel->activeEventFor( id );
            const bool isActive = activeEvent.isValid();
            if ( isActive ) {
                flags |= Qt::ItemIsEditable;
            }
        }
    }

    return flags;
}

bool TaskModelAdapter::setData( const QModelIndex & index, const QVariant & value, int role )
{
    if ( ! index.isValid() )
        return false;

    const TaskTreeItem* item = itemFor ( index );
    Q_ASSERT( item != 0 );
    Task task( item->task() ); // make a copy, so that we can modify it

    if ( role == Qt::EditRole ) {
        Q_ASSERT( m_dataModel->isTaskActive( task.id() ) );

        const Event& old = m_dataModel->activeEventFor ( task.id() );
        QString comment = value.toString();
        Event event( old );
        event.setComment( comment );
        CommandModifyEvent* command = new CommandModifyEvent( event, old, this );
        VIEW.sendCommand( command );
        return true;
    } else if ( role == Qt::CheckStateRole ) {
        task.setSubscribed( ! task.subscribed() );
        CommandModifyTask* command = new CommandModifyTask( task, this );
        VIEW.sendCommand( command );
        return true;
    }
    return false;
}

void TaskModelAdapter::resetTasks()
{
    reset();
}

void TaskModelAdapter::taskAboutToBeAdded( TaskId parentId, int pos )
{
    const TaskTreeItem& parent = m_dataModel->taskTreeItem( parentId );
    beginInsertRows( indexForTaskTreeItem( parent, 0 ), pos, pos );
}

void TaskModelAdapter::taskAdded( TaskId id )
{
    endInsertRows();
}

void TaskModelAdapter::taskParentChanged( TaskId task, TaskId oldParent, TaskId newParent )
{
	// remove task from old parent:
	const TaskTreeItem& item = m_dataModel->taskTreeItem( task );
	const int row = item.row();
	const TaskTreeItem& oldParentItem = m_dataModel->taskTreeItem( oldParent );
	beginRemoveRows( indexForTaskTreeItem( oldParentItem, 0 ), row, row );
	// the actual remove happens in the data model
	endRemoveRows();
	// add task to new patent:
	const TaskTreeItem& newParentItem = m_dataModel->taskTreeItem( newParent );
	const int pos = newParentItem.childCount();
	beginInsertRows( indexForTaskTreeItem( newParentItem, 0 ), pos, pos );
	// the actual insert happens in the data model
	endInsertRows();
}


void TaskModelAdapter::taskModified( TaskId id )
{
    const TaskTreeItem& item = m_dataModel->taskTreeItem( id );
    if ( item.isValid() ) {
        QModelIndex startIndex = indexForTaskTreeItem( item, 0 );
        QModelIndex endIndex = indexForTaskTreeItem( item, Column_TaskId );
        emit dataChanged( startIndex, endIndex );
    }
}

void TaskModelAdapter::taskAboutToBeDeleted( TaskId id )
{
    const TaskTreeItem& item = m_dataModel->taskTreeItem( id );
    const TaskTreeItem& parent = m_dataModel->parentItem( item.task() );
    int row = item.row();
    Q_ASSERT( row != -1 );

    beginRemoveRows( indexForTaskTreeItem( parent, 0 ), row, row );
}

void TaskModelAdapter::taskDeleted( TaskId id )
{
    endRemoveRows();
}

void TaskModelAdapter::eventAdded( EventId id )
{
    const Event& event = m_dataModel->eventForId( id );
    taskModified( event.taskId() );
}

void TaskModelAdapter::eventModified( EventId id, Event oldEvent )
{
    const Event& event = m_dataModel->eventForId( id );
    const TaskTreeItem& item = m_dataModel->taskTreeItem( event.taskId() );

    if ( item.isValid() ) {
        // find out about what fields have actually changed, so that no
        // ongoing edits are overridden (to fix till' s bug report)
        // -- DF: we can't do that anymore, with a single column.
        // see TasksViewDelegate::setEditorData for the fix.

        QModelIndex startIndex = indexForTaskTreeItem( item, 0 );
        QModelIndex endIndex = indexForTaskTreeItem( item, Column_TaskId );
        emit dataChanged( startIndex, endIndex );
    }
}

void TaskModelAdapter::eventDeleted( EventId id )
{
    eventAdded( id );
}

void TaskModelAdapter::eventActivated( EventId id )
{
    // query the model to find out the task:
    const Event& event = m_dataModel->eventForId( id );
    if ( event.isValid() ) {
        taskModified( event.taskId() );
        emit eventActivationNotice( id );
    }
}

void TaskModelAdapter::eventDeactivated( EventId id )
{
    // query the model to find out the task:
    const Event& event = m_dataModel->eventForId( id );
    if ( event.isValid() ) {
        taskModified( event.taskId() );
        emit eventDeactivationNotice( id );
    }
}

const TaskTreeItem* TaskModelAdapter::itemFor ( const QModelIndex& index ) const
{
    if ( index.isValid() ) {
        return static_cast<TaskTreeItem*>( index.internalPointer() );
    } else {
        return &m_dataModel->taskTreeItem( 0 );
    }
}

QModelIndex TaskModelAdapter::indexForTaskTreeItem( const TaskTreeItem& item,
                                                    int column ) const
{
    if ( item.isValid() ) {
        // argl UUUUGGGLLYYYY
        // DF: how about reinterpret_cast<void*>(&item) ?
        const void* constVoidPointer = static_cast<const void*>( &item );
        void* voidPointer = const_cast<void*>( constVoidPointer );
        return createIndex( item.row(), column, voidPointer );
    } else {
        return QModelIndex();
    }
}

QModelIndex TaskModelAdapter::indexForTaskId( TaskId id ) const
{
    return indexForTaskTreeItem( m_dataModel->taskTreeItem( id ) );
}

Task TaskModelAdapter::taskForIndex( const QModelIndex& index ) const
{
    const TaskTreeItem* item = itemFor ( index );
    return item->task();
}

bool TaskModelAdapter::taskIsActive( const Task& task ) const
{
    Q_ASSERT( m_dataModel != 0 );
    return m_dataModel->isTaskActive( task.id() );
}

bool TaskModelAdapter::taskHasChildren( const Task& task ) const
{
    const TaskTreeItem& item = m_dataModel->taskTreeItem( task.id() );
    return item.childCount() > 0;
}

bool TaskModelAdapter::taskIdExists( TaskId taskId ) const
{
    return m_dataModel->taskExists( taskId );
}

void TaskModelAdapter::commitCommand( CharmCommand* command )
{
    Q_ASSERT( command->owner() == this );
    command->finalize();
}

#include "TaskModelAdapter.moc"
