/*
  ViewFilter.cpp

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

#include "ViewFilter.h"
#include "Core/CharmDataModel.h"
#include "ViewHelpers.h"

ViewFilter::ViewFilter( CharmDataModel* model, QObject* parent )
    : QSortFilterProxyModel( parent )
    , m_model( model )
{
    setSourceModel( &m_model );

    // we filter for the task name column
    setFilterKeyColumn( Column_TaskId );
    // setFilterKeyColumn( -1 );
    setFilterCaseSensitivity( Qt::CaseInsensitive );

    // relay signals to the view:
    connect( &m_model, SIGNAL(eventActivationNotice(EventId)),
             SIGNAL(eventActivationNotice(EventId)) );
    connect( &m_model, SIGNAL(eventDeactivationNotice(EventId)),
             SIGNAL(eventDeactivationNotice(EventId)) );

    sort( Column_TaskId );
}

ViewFilter::~ViewFilter()
{
}

Task ViewFilter::taskForIndex( const QModelIndex& index ) const
{
    return m_model.taskForIndex( mapToSource( index ) );
}

QModelIndex ViewFilter::indexForTaskId( TaskId id ) const
{
    return mapFromSource( m_model.indexForTaskId( id ) );
}

bool ViewFilter::taskIsActive( const Task& task ) const
{
    return m_model.taskIsActive( task );
}

bool ViewFilter::taskHasChildren( const Task& task ) const
{
    return m_model.taskHasChildren( task );
}

void ViewFilter::prefilteringModeChanged()
{
    invalidate();
}

bool ViewFilter::filterAcceptsRow( int source_row, const QModelIndex& parent ) const
{
    // by default, QSortFilterProxyModel only accepts row where already the parents where accepted
    bool acceptedByFilter = QSortFilterProxyModel::filterAcceptsRow( source_row, parent );
    // in our case, this is not what we want, we want parents to be
    // accepted if any of their children are accepted (this is a
    // recursive call, and could possibly be slow):
    const QModelIndex index( m_model.index( source_row, 0, parent ) );
    if ( ! index.isValid() ) return acceptedByFilter;

    int rowCount = m_model.rowCount( index );
    for ( int i = 0; i < rowCount; ++i ) {
        if ( filterAcceptsRow( i, index ) ) {
            acceptedByFilter = true;
            break;
        }
    }

    bool accepted = acceptedByFilter;
    const Task task = m_model.taskForIndex( index );
    switch( Configuration::instance().taskPrefilteringMode ) {
    case Configuration::TaskPrefilter_ShowAll:
        break;
    case Configuration::TaskPrefilter_CurrentOnly: {
        const bool ok = ( task.isCurrentlyValid() || hasValidChildren( task ) );
        accepted &= ok;
        break;
    }
    case Configuration::TaskPrefilter_SubscribedOnly:
        accepted &= task.subscribed();
        break;
    case Configuration::TaskPrefilter_SubscribedAndCurrentOnly:
        accepted &= ( task.subscribed() && task.isCurrentlyValid() );
        break;
    default:
        break;
    }

    return accepted;
}

bool ViewFilter::filterAcceptsColumn( int source_column, const QModelIndex& ) const
{
    return true;
}

bool ViewFilter::taskIdExists( TaskId taskId ) const
{
    return m_model.taskIdExists( taskId );
}

bool ViewFilter::hasValidChildren( Task task ) const
{
    if ( taskHasChildren( task ) ) {
        const TaskIdList idList = m_model.childrenIds( task );
        for ( int i = 0; i < idList.count(); ++i ) {
            const Task childTask = DATAMODEL->getTask( idList[i] );
            if ( childTask.isCurrentlyValid() ) {
                return true;
            }
        }
    }
    return false;
}

void ViewFilter::commitCommand( CharmCommand* command )
{   // we do not emit signals, we are the relay (since we are a proxy):
    m_model.commitCommand( command );
}
#include "moc_ViewFilter.cpp"
