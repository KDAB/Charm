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
    , m_filterId()
{
    setSourceModel( &m_model );
    setDynamicSortFilter( true );
    sort( 0, Qt::AscendingOrder );

    connect( &m_model, SIGNAL(eventActivationNotice(EventId)),
             SIGNAL(eventActivationNotice(EventId)) );
    connect( &m_model, SIGNAL(eventDeactivationNotice(EventId)),
             SIGNAL(eventDeactivationNotice(EventId)) );
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
    if ( QSortFilterProxyModel::filterAcceptsRow( srow, sparent ) == false ) {
        return false;
    }

    const Event& event = m_model.eventForIndex( m_model.index( srow, 0, sparent ) );

    if ( m_filterId != TaskId() && event.taskId() != m_filterId ) {
        return false;
    }

    const auto startDate = event.startDateTime().date();
    if ( m_start.isValid() && startDate < m_start ) {
        return false;
    }

    if ( m_end.isValid() && startDate >= m_end ) {
        return false;
    }

    return true;
}

void EventModelFilter::setFilterStartDate( const QDate& date )
{
    if ( m_start == date )
        return;
    m_start = date;
    invalidateFilter();
}

void EventModelFilter::setFilterEndDate( const QDate& date )
{
    if ( m_end == date )
        return;
    m_end = date;
    invalidateFilter();
}

void EventModelFilter::setFilterTaskId( TaskId id )
{
    if ( m_filterId == id )
        return;
    m_filterId = id;
    invalidateFilter();
}

int EventModelFilter::totalDuration() const
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

QList<Event> EventModelFilter::events() const
{
    QList<Event> events;
    for ( int i = 0; i < rowCount(); ++i )
    {
        QModelIndex hit = index( i, 0, QModelIndex() );
        const Event& event = eventForIndex( hit );
        events << event;
    }
    return events;
}


#include "moc_EventModelFilter.cpp"

