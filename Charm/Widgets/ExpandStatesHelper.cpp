/*
  ExpandStatesHelper.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

#include "ExpandStatesHelper.h"
#include "TaskModelAdapter.h"

#include <QTreeView>

static void saveChildExpandStates( const QModelIndex& idx, QTreeView* tv, QHash<TaskId,bool>* map ) {
    const int rc = idx.model()->rowCount( idx );
    for ( int i = 0; i < rc; ++i ) {
        const QModelIndex c = idx.child( i, 0 );
        if ( c.model()->rowCount( c ) > 0 ) {
            const TaskId id = c.data( TasksViewRole_TaskId ).toInt();
            const bool expanded = tv->isExpanded( c );
            map->insert( id, expanded );
            if ( expanded )
                saveChildExpandStates( c, tv, map );
        }
    }
}

static void restoreChildExpandStates( const QModelIndex& idx, QTreeView* tv, QHash<TaskId,bool>* map ) {
    const int rc = idx.model()->rowCount( idx );
    for ( int i = 0; i < rc; ++i ) {
        const QModelIndex c = idx.child( i, 0 );
        if ( c.model()->rowCount( c ) > 0 ) {
            const TaskId id = c.data( TasksViewRole_TaskId ).toInt();
            const bool expanded = map->value( id );
            tv->setExpanded( c, expanded );
            restoreChildExpandStates( c, tv, map );
        }
    }
}

void Charm::saveExpandStates( QTreeView* tv, QHash<TaskId,bool>* map ) {
    if ( !tv->model() )
        return;
    if ( tv->model()->rowCount() == 0 )
        return;
    const QModelIndex root = tv->model()->index( 0, 0, QModelIndex() );
    const TaskId id = root.data( TasksViewRole_TaskId ).toInt();
    const bool expanded = tv->isExpanded( root );
    map->insert( id, expanded );
    saveChildExpandStates( root, tv, map );
}

void Charm::restoreExpandStates(  QTreeView* tv, QHash<TaskId,bool>* map ) {
    if ( !tv->model() )
        return;
    if ( tv->model()->rowCount() == 0 )
        return;
    const QModelIndex root = tv->model()->index( 0, 0, QModelIndex() );
    const TaskId id = root.data( TasksViewRole_TaskId ).toInt();
    const bool expand = map->value( id );
    tv->setExpanded( root, expand );
    restoreChildExpandStates( root, tv, map );
}
