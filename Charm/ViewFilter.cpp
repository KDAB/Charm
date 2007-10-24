#include "Core/CharmDataModel.h"
#include "ViewFilter.h"

ViewFilter::ViewFilter( CharmDataModel* model, QObject* parent )
    : QSortFilterProxyModel( parent )
    , m_model( model )
    , m_subscribedTasksOnly( false )
{
    setSourceModel ( &m_model );

    // we filter for the task name column
    setFilterKeyColumn( Column_TaskName );
    // setFilterKeyColumn( -1 );
    setFilterCaseSensitivity( Qt::CaseInsensitive );

    // relay signals to the view:
    connect( &m_model, SIGNAL( eventActivationNotice( EventId ) ),
             SIGNAL( eventActivationNotice( EventId ) ) );
    connect( &m_model, SIGNAL( eventDeactivationNotice( EventId ) ),
             SIGNAL( eventDeactivationNotice( EventId ) ) );

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

void ViewFilter::setSubscribedTasksOnlyMode( bool onoff )
{
    m_subscribedTasksOnly = onoff;
    clear();
}

bool ViewFilter::filterAcceptsRow( int source_row, const QModelIndex& parent ) const
{
    // by default, QSortFilterProxyModel only accepts row where already the parents where accepted
    bool acceptedByFilter = QSortFilterProxyModel::filterAcceptsRow( source_row, parent );
    // in our case, this is not what we want, we want parents to be
    // accepted if any of their children are accepted (this is a
    // recursive call, and could possibly be slow):
    const QModelIndex index( m_model.index( source_row, 0, parent ) );
    int rowCount = m_model.rowCount( index );
    for ( int i = 0; i < rowCount; ++i ) {
        if ( filterAcceptsRow( i, index ) ) {
            acceptedByFilter = true;
            break;
        }
    }

    bool accepted = acceptedByFilter;
    if ( m_subscribedTasksOnly )
    {
        Q_ASSERT( index.isValid() );
        Task task = m_model.taskForIndex( index );
        accepted = task.subscribed() && acceptedByFilter;
    }

    return accepted;
}

bool ViewFilter::taskIdExists( TaskId taskId ) const
{
    return m_model.taskIdExists( taskId );
}

void ViewFilter::commitCommand( CharmCommand* command )
{   // we do not emit signals, we are the relay (since we are a proxy):
    m_model.commitCommand( command );
}

#include "ViewFilter.moc"
