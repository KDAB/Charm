/*
  EventModelFilter.cpp

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

#include "EventModelFilter.h"

EventModelFilter::EventModelFilter( CharmDataModel* model, QObject* parent )
    : QSortFilterProxyModel( parent )
    , m_model( model )
{
    setSourceModel( &m_model );
    setDynamicSortFilter( true );
    sort( 0, Qt::AscendingOrder );

    connect( &m_model, SIGNAL( eventActivationNotice( EventId ) ),
             SIGNAL( eventActivationNotice( EventId ) ) );
    connect( &m_model, SIGNAL( eventDeactivationNotice( EventId ) ),
             SIGNAL( eventDeactivationNotice( EventId ) ) );
}

EventModelFilter::~EventModelFilter()
{
}

void EventModelFilter::commitCommand( CharmCommand* command )
{
    m_model.commitCommand( command );
}

bool EventModelFilter::lessThan( const QModelIndex& left, const QModelIndex& right ) const
{
    if ( left.column() == 0 && right.column() == 0 ) {
        const Event& leftEvent = m_model.eventForIndex( left );
        const Event& rightEvent = m_model.eventForIndex( right );
        return leftEvent.startDateTime() < rightEvent.startDateTime();
    } else {
        return QSortFilterProxyModel::lessThan( left, right );
    }
}

const Event& EventModelFilter::eventForIndex( const QModelIndex& index ) const
{
    return m_model.eventForIndex( mapToSource( index ) );
}

QModelIndex EventModelFilter::indexForEvent( const Event& event ) const
{
    const QModelIndex& sourceIndex = m_model.indexForEvent( event );
    const QModelIndex& proxyIndex( mapFromSource( sourceIndex ) );
    // bool valid = proxyIndex.isValid();
    return proxyIndex;
}

bool EventModelFilter::filterAcceptsRow( int srow, const QModelIndex& sparent ) const
{
    const Event& event = m_model.eventForIndex( m_model.index( srow, 0, sparent ) );
    return ( event.startDateTime().date() >= m_start
             && event.startDateTime().date() < m_end );
}

void EventModelFilter::setFilterStartDate( const QDate& date )
{
    m_start = date;
    filterChanged();
}

void EventModelFilter::setFilterEndDate( const QDate& date )
{
    m_end = date;
    filterChanged();
}

int EventModelFilter::totalDuration()
{
    int total = 0;
    for ( int i = 0; i < rowCount(); ++i )
    {
        QModelIndex current = index( i, 0, QModelIndex() );
        const Event& event = eventForIndex( current );
        total += event.duration();
    }
    return total;
}

#include "moc_EventModelFilter.cpp"

