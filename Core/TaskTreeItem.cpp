#include <QtDebug>

#include "TaskTreeItem.h"

TaskTreeItem::TaskTreeItem()
    : m_parent( 0 )
{
}

TaskTreeItem::TaskTreeItem( const Task& task, TaskTreeItem* parent )
    : m_parent( parent )
    , m_task( task )
{
    if ( m_parent ) {
        m_parent->m_children.append( this );
    }
}

TaskTreeItem::TaskTreeItem( const TaskTreeItem& other )
{
    *this = other;
}

void TaskTreeItem::operator=( const TaskTreeItem& other )
{
    m_children = other.m_children;
    m_parent = other.m_parent;
    m_task = other.m_task;
    if ( m_parent ) {
        m_parent->m_children.append( this );
    }
}

TaskTreeItem::~TaskTreeItem()
{
    if ( m_parent ) {
        m_parent->m_children.removeAt( row() );
    }
}

void TaskTreeItem::makeChildOf( TaskTreeItem& parent )
{
    if ( m_parent != &parent ) {
        Q_ASSERT( ! parent.m_children.contains( this ) ); // that would be wrong

        // if there is an existing parent, unregister with it:
        // parent can only be zero if there never was a parent so far
        if ( m_parent != 0 ) {
            m_parent->m_children.removeAt( row() );
            m_parent = 0;
        }

        // register with the new parent
        m_parent = &parent;
        parent.m_children.append( this );
    } else {
        // hm, should this be allowed?
        // done
    }
}

Task& TaskTreeItem::task()
{
    return m_task;
}

const Task& TaskTreeItem::task() const
{
    return m_task;
}

bool TaskTreeItem::isValid() const
{
    return m_parent != 0 && m_task.isValid();
}

const TaskTreeItem& TaskTreeItem::child( int row ) const
{
    static TaskTreeItem InvalidItem;

    if ( row >= 0 && row < m_children.size() ) {
        return * m_children.at( row );
    } else {
        Q_ASSERT_X( false, "TaskTreeItem::child", "Invalid item position" );
        return InvalidItem;
    }
}

int TaskTreeItem::row() const
{
    if ( m_parent ) {
        int row = m_parent->m_children.indexOf( this );
        Q_ASSERT_X( row != -1, "TaskTreeItem::row",
                    "Internal error - cannot find myself in my parents family" );
        return row;
    } else {
        Q_ASSERT_X( false, "TaskTreeItem::row",
                    "Calling row() on an invalid item" );
        return -1;
    }
}

int TaskTreeItem::childCount() const
{
    return m_children.size();
}

TaskList TaskTreeItem::children() const
{
    TaskList tasks;
    for ( int i = 0; i < m_children.size(); ++i )
    {
        tasks << m_children[i]->task()
              << m_children[i]->children();
    }

    return tasks;
}

TaskIdList TaskTreeItem::childIds() const
{
    TaskIdList idList;
    // get the list of children, and sort by task id:
    Q_FOREACH( const TaskTreeItem* item, m_children ) {
        idList.append( item->task().id() );
    }
    return idList;
}
