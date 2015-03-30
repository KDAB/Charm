/*
  TaskTreeItem.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2007-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>
  Author: Frank Osterfeld <frank.osterfeld@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "TaskTreeItem.h"

#include <QtDebug>

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
    if( this != &other ) {
        *this = other;
    }
}

TaskTreeItem& TaskTreeItem::operator=( const TaskTreeItem& other )
{
    if( this != &other ) {
        m_children = other.m_children;
        m_parent = other.m_parent;
        m_task = other.m_task;
        if ( m_parent ) {
            m_parent->m_children.append( this );
        }
    }
    return *this;
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
            m_parent = nullptr;
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
        Q_ASSERT_X( false, Q_FUNC_INFO, "Invalid item position" );
        return InvalidItem;
    }
}

int TaskTreeItem::row() const
{
    if ( m_parent ) {
        int row = m_parent->m_children.indexOf( this );
        Q_ASSERT_X( row != -1, Q_FUNC_INFO,
                    "Internal error - cannot find myself in my parents family" );
        return row;
    } else {
        Q_ASSERT_X( false, Q_FUNC_INFO,
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
