#include <Core/CharmDataModel.h>
#include <Core/CharmCommand.h>

#include "EventModelAdapter.h"

EventModelAdapter::EventModelAdapter( CharmDataModel* parent )
    : QAbstractListModel( parent )
    , m_dataModel( parent )
{
    m_dataModel->registerAdapter( this );
}

EventModelAdapter::~EventModelAdapter()
{
    m_dataModel->unregisterAdapter( this );
}

int EventModelAdapter::rowCount( const QModelIndex& ) const
{
    return m_events.size();
}

QVariant EventModelAdapter::data( const QModelIndex& index, int role ) const
{
    if ( !index.isValid() || index.row() < 0 || index.row() > m_events.size() )
        return QVariant(); // beware of stale persistent indexes

    switch ( role ) {
    case Qt::DisplayRole:
    {
        EventId eventId = m_events[index.row()];
        const Event& event = m_dataModel->eventForId( eventId );

        QString text;
        QTextStream stream( &text );
        stream << event.taskId() << " - " << event.comment();

        return text;
    }
    break;

    default:
        return QVariant();
    }
}

void EventModelAdapter::resetEvents()
{
    beginResetModel();

    m_events.clear();

    for ( EventMap::const_iterator it = m_dataModel->eventMap().begin();
          it != m_dataModel->eventMap().end(); ++it ) {
        m_events.append( it->first );
    }

    endResetModel();
}

void EventModelAdapter::eventAboutToBeAdded( EventId id )
{
    int position = m_events.size();
    beginInsertRows( QModelIndex(), position, position );
}

void EventModelAdapter::eventAdded( EventId id )
{
    m_events.append( id );
    endInsertRows();
}

void EventModelAdapter::eventModified( EventId id, Event )
{
    // nothing to do, except:
    int row = m_events.indexOf( id );
    Q_ASSERT( row != -1 ); // inconsistency between model and adapter
    emit( dataChanged( index( row ), index( row ) ) );
}

void EventModelAdapter::eventAboutToBeDeleted( EventId id )
{
    int row = m_events.indexOf( id );
    Q_ASSERT( row != -1 ); // inconsistency between model and adapter
    beginRemoveRows( QModelIndex(), row, row );
}

void EventModelAdapter::eventDeleted( EventId id )
{
    int position = m_events.indexOf( id );
    Q_ASSERT( position != -1 ); // inconsistency between model and adapter
    m_events.removeAt( position );
    Q_ASSERT( m_events.indexOf(  id ) == -1 ); // cannot be in there
    endRemoveRows();
}

void EventModelAdapter::eventActivated( EventId id )
{
    emit eventActivationNotice( id );
}

void EventModelAdapter::eventDeactivated( EventId id )
{
    emit eventDeactivationNotice( id );
}

void EventModelAdapter::commitCommand( CharmCommand* command )
{
    command->finalize();
}

const Event& EventModelAdapter::eventForIndex( const QModelIndex& index ) const
{
    if ( index.row() >=0 && index.row() < m_events.size() ) {
        EventId eventId = m_events.at( index.row() );
        const Event& event = m_dataModel->eventForId( eventId );
        return event;
    } else {
        static Event InvalidEvent;
        return InvalidEvent;
    }
}

QModelIndex EventModelAdapter::indexForEvent( const Event& event ) const
{
    int position = m_events.indexOf( event.id() );

    if ( position >= 0 && position < m_events.size() ) {
        return index( position );
    } else {
        return QModelIndex();
    }
}

#include "moc_EventModelAdapter.cpp"
