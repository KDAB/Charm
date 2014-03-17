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
