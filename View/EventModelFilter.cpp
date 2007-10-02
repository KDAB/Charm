#include <QTimer>

#include "EventModelFilter.h"

EventModelFilter::EventModelFilter( CharmDataModel* model, QObject* parent )
    : QSortFilterProxyModel( parent )
    , m_model( model )
{
    setSourceModel( &m_model );
    setDynamicSortFilter( true );
    sort( 0 );

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
    return mapFromSource( m_model.indexForEvent( event ) );
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

#include "EventModelFilter.moc"

