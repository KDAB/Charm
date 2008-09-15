#include <algorithm>

#include <QList>
#include <QtDebug>
#include <QDateTime>

#include "CharmConstants.h"
#include "Configuration.h"
#include "CharmDataModel.h"

CharmDataModel::CharmDataModel()
    : QObject()
{
    connect( &m_timer, SIGNAL( timeout() ), SLOT( eventUpdateTimerEvent() ) );
}

CharmDataModel::~CharmDataModel()
{
}

void CharmDataModel::stateChanged( State previous, State next )
{
    if ( previous == Connected && next == Disconnecting ) {
        Q_FOREACH( EventId id, m_activeEventIds ) {
            Event& event = findEvent( id );
            Task& task = findTask( event.taskId() );
            Q_ASSERT( task.isValid() );
            endEventRequested( task );
        }
    }
}

void CharmDataModel::registerAdapter( CharmDataModelAdapterInterface* adapter )
{
    m_adapters.append( adapter );
}

void CharmDataModel::unregisterAdapter( CharmDataModelAdapterInterface* adapter )
{
    Q_ASSERT( m_adapters.contains( adapter ) );
    m_adapters.removeAll( adapter );
}

void CharmDataModel::setAllTasks( const TaskList& tasks )
{
    m_tasks.clear();
    m_rootItem = TaskTreeItem();

    // fill the tasks into the map to TaskTreeItems
    for ( int i = 0; i < tasks.size(); ++i )
    {
        TaskTreeItem item( tasks[i] );

        if ( ! taskExists( tasks[i].id() ) ) {
            m_tasks[ tasks[i].id() ] = item;
        } else {
            qDebug() << "CharmDataModel::setAllTasks: duplicate task id"
                     << tasks[i].id() << "ignored. THIS IS A BUG";
            Q_ASSERT_X( false, "CharmDataModel::setAllTasks",
                        "Task list contains duplicate ids" );
        }
    }

    // create parent-child-relationships:
    for ( TaskTreeItem::Map::iterator it = m_tasks.begin(); it != m_tasks.end(); ++it )
    {
        it->second.makeChildOf( parentItem( it->second.task() ) );
    }

    // store task id length:
    determineTaskPaddingLength();

    // FIXME check tree for treeness, e.g. no loops

    // notify adapters of changes
    for_each( m_adapters.begin(), m_adapters.end(),
              std::mem_fun( &CharmDataModelAdapterInterface::resetTasks ) );
}

void CharmDataModel::addTask( const Task& task )
{
    Q_ASSERT_X( ! taskExists( task.id() ), " CharmDataModel::addTask",
                "New tasks need to have a unique task id" );

    if ( task.isValid() && ! taskExists( task.id() ) ) {
        const TaskTreeItem& parent = taskTreeItem( task.parent() );

        Q_FOREACH( CharmDataModelAdapterInterface* adapter, m_adapters )
            adapter->taskAboutToBeAdded( parent.task().id(),
                                         parent.childCount() );

        TaskTreeItem item ( task );
        m_tasks[ task.id() ] = item;

        // the item in the map has a different address, let's find it:
        Q_ASSERT( taskExists( task.id() ) ); // we just put it in
        TaskTreeItem::Map::iterator it = m_tasks.find( task.id() );
        it->second.makeChildOf( parentItem( task ) );

        determineTaskPaddingLength();

        Q_FOREACH( CharmDataModelAdapterInterface* adapter, m_adapters )
            adapter->taskAdded( task.id() );
    } else {
        qDebug() << "CharmDataModel::addTask: duplicate task id"
                 << task.id() << "ignored. THIS IS A BUG";
    }
}

void CharmDataModel::modifyTask( const Task& task )
{
    TaskTreeItem::Map::iterator it = m_tasks.find( task.id() );
    Q_ASSERT_X( it != m_tasks.end(), "CharmDataModel::modifyTask",
              "Task to modify has to exist" );

    if ( it != m_tasks.end() ) {
        TaskId oldParentId = it->second.task().parent();
        bool parentChanged = task.parent() != oldParentId;

        if ( parentChanged ) {
            Q_FOREACH( CharmDataModelAdapterInterface* adapter, m_adapters )
				adapter->taskParentChanged( task.id(), oldParentId, task.parent() );
            m_tasks[ task.id() ].makeChildOf( parentItem( task ) );
        }

        m_tasks[ task.id() ].task() = task;

        if( parentChanged ) {
            Q_FOREACH( CharmDataModelAdapterInterface* adapter, m_adapters )
                adapter->resetTasks();
        } else {
        	Q_FOREACH( CharmDataModelAdapterInterface* adapter, m_adapters )
            adapter->taskModified( task.id() );
        }
    }
}

void CharmDataModel::deleteTask( const Task& task )
{
    Q_ASSERT_X( taskExists( task.id() ), "CharmDataModel::deleteTask",
                "Task to delete has to exist" );
    Q_ASSERT_X( taskTreeItem( task.id() ).childCount() == 0,
                "CharmDataModel::deleteTask",
                "Cannot delete a task that has children" );

    Q_FOREACH( CharmDataModelAdapterInterface* adapter, m_adapters )
        adapter->taskAboutToBeDeleted( task.id() );

    TaskTreeItem::Map::iterator it = m_tasks.find( task.id() );
    if ( it != m_tasks.end() ) {
        TaskTreeItem tmpParent;
        it->second.makeChildOf( tmpParent );
        m_tasks.erase( it );
    }

    Q_FOREACH( CharmDataModelAdapterInterface* adapter, m_adapters )
        adapter->taskDeleted( task.id() );
}

void CharmDataModel::clearTasks()
{
    m_tasks.clear();
    m_rootItem = TaskTreeItem();

    Q_FOREACH( CharmDataModelAdapterInterface* adapter, m_adapters )
        adapter->resetTasks();
}

void CharmDataModel::setAllEvents( const EventList& events )
{
    m_events.clear();

    for ( int i = 0; i < events.size(); ++i )
    {
        if ( ! eventExists( events[i].id() ) ) {
            m_events[ events[i].id() ] = events[i];
        } else {
            qDebug() << "CharmDataModel::addTask: duplicate task id"
                     << m_tasks[i].task().id() << "ignored. THIS IS A BUG";
        }
    }

    Q_FOREACH( CharmDataModelAdapterInterface* adapter, m_adapters )
        adapter->resetEvents();
}

void CharmDataModel::addEvent( const Event& event )
{
    Q_ASSERT_X( ! eventExists( event.id() ), "CharmDataModel::addEvent",
                "New event must have a unique id" );

    Q_FOREACH( CharmDataModelAdapterInterface* adapter, m_adapters )
        adapter->eventAboutToBeAdded( event.id() );

    m_events[ event.id() ] = event;

    Q_FOREACH( CharmDataModelAdapterInterface* adapter, m_adapters )
        adapter->eventAdded( event.id() );
}

void CharmDataModel::modifyEvent( const Event& newEvent )
{
    Q_ASSERT_X( eventExists( newEvent.id() ), "CharmDataModel::modifyEvent",
                "Event to modify has to exist" );

    Event oldEvent = eventForId( newEvent.id() );

    m_events[ newEvent.id() ] = newEvent;

    Q_FOREACH( CharmDataModelAdapterInterface* adapter, m_adapters )
        adapter->eventModified( newEvent.id(), oldEvent );
}

void CharmDataModel::deleteEvent( const Event& event )
{
    Q_ASSERT_X( eventExists( event.id() ), "CharmDataModel::deleteEvent",
                "Event to delete has to exist" );
    Q_ASSERT_X( !m_activeEventIds.contains( event.id() ), "CharmDataModel::deleteEvent",
                "Cannot delete an active event" );

    Q_FOREACH( CharmDataModelAdapterInterface* adapter, m_adapters )
        adapter->eventAboutToBeDeleted( event.id() );

    EventMap::iterator it = m_events.find( event.id() );
    if ( it != m_events.end() )
        m_events.erase( it );

    Q_FOREACH( CharmDataModelAdapterInterface* adapter, m_adapters )
        adapter->eventDeleted( event.id() );
}

void CharmDataModel::clearEvents()
{
    m_events.clear();

    Q_FOREACH( CharmDataModelAdapterInterface* adapter, m_adapters )
        adapter->resetEvents();
}

const TaskTreeItem& CharmDataModel::taskTreeItem( TaskId id ) const
{
    if ( id <= 0 ) return m_rootItem;

    TaskTreeItem::Map::const_iterator it = m_tasks.find( id );
    if ( it == m_tasks.end() ) {
        return m_rootItem;
    } else {
        return it->second;
    }
}

const Task& CharmDataModel::getTask( TaskId id ) const
{
    const TaskTreeItem& item = taskTreeItem( id );
    return item.task();
}

TaskList CharmDataModel::getAllTasks() const
{
    return m_rootItem.children();
}

Task& CharmDataModel::findTask( TaskId id )
{   // in this (private) method, the task has to exist
    TaskTreeItem::Map::iterator it = m_tasks.find( id );
    Q_ASSERT( it != m_tasks.end() );
    return it->second.task();
}

const Event& CharmDataModel::eventForId( EventId id ) const
{
    static Event InvalidEvent;
    EventMap::const_iterator it = m_events.find( id );
    if ( it != m_events.end() ) {
        return it->second;
    } else {
        return InvalidEvent;
    }
}

Event& CharmDataModel::findEvent( int id )
{
    // in this method, the event has to exist
    EventMap::iterator it = m_events.find( id );
    Q_ASSERT( it != m_events.end() );
    return it->second;
}

bool CharmDataModel::activateEvent( const Event& activeEvent )
{
    const bool DoSanityChecks = true;
    if ( DoSanityChecks ) {
        TaskId taskId = activeEvent.taskId();

        // this check may become obsolete:
        for ( int i = 0; i < m_activeEventIds.size(); ++i )
        {
            if ( m_activeEventIds[i] == activeEvent.id() ) {
                Q_ASSERT( !"inconsistency (event already active)!" );
                return false;
            }

            const Event& e = eventForId( m_activeEventIds[i] );
            if ( e.taskId() == taskId ) {
                Q_ASSERT( !"inconsistency (event already active for task)!" );
                return false;
            }
        }

    }

    m_activeEventIds << activeEvent.id();
    Q_FOREACH( CharmDataModelAdapterInterface* adapter, m_adapters ) {
        adapter->eventActivated( activeEvent.id() );
    }
    m_timer.start( 10000 );
    return true;
}

void CharmDataModel::determineTaskPaddingLength()
{
    int maxTaskId = 0;

    for ( TaskTreeItem::Map::iterator it = m_tasks.begin(); it != m_tasks.end(); ++it )
    {
        maxTaskId = qMax( maxTaskId, it->second.task().id() );
    }

    QString temp;
    temp.setNum( maxTaskId );
    CONFIGURATION.taskPaddingLength = temp.length();
}

TaskTreeItem& CharmDataModel::parentItem( const Task& task )
{
    if ( m_tasks[ task.parent() ].isValid() ) {
        return m_tasks[ task.parent() ];
    } else {
        return m_rootItem;
    }
}

bool CharmDataModel::taskExists( TaskId id )
{
    return m_tasks.find( id ) != m_tasks.end();
}

bool CharmDataModel::eventExists( EventId id )
{
    return m_events.find( id ) != m_events.end();
}

bool CharmDataModel::isTaskActive( TaskId id ) const
{
    for ( int i = 0; i < m_activeEventIds.size(); ++i )
    {
        const Event& e = eventForId( m_activeEventIds[i] );
        Q_ASSERT( e.isValid() );
        if ( e.taskId() == id ) {
            return true;
        }
    }

    return false;
}

const Event& CharmDataModel::activeEventFor ( TaskId id ) const
{
    static Event InvalidEvent;

    for ( int i = 0; i < m_activeEventIds.size(); ++i )
    {
        const Event& e = eventForId( m_activeEventIds[i] );
        if ( e.taskId() == id ) {
            return e;
        }
    }

    return InvalidEvent;
}

void CharmDataModel::startEventRequested( const Task& task )
{
    // respect configuration:
    if ( CONFIGURATION.oneEventAtATime && !m_activeEventIds.isEmpty() ) {
        endAllEventsRequested();
    }

    emit makeAndActivateEvent( task );
}

void CharmDataModel::endEventRequested( const Task& task )
{
    EventId eventId = 0;

    // find the event in the list of active events and remove it:
    for ( int i = 0; i < m_activeEventIds.size(); ++i )
    {
        if ( eventForId( m_activeEventIds[i] ).taskId() == task.id() ) {
            eventId = m_activeEventIds[i];
            m_activeEventIds.removeAt( i );
            Q_FOREACH( CharmDataModelAdapterInterface* adapter, m_adapters ) {
                adapter->eventDeactivated( eventId );
            }
            break;
        }
    }

    Q_ASSERT( eventId != 0 );
    Event& event = findEvent( eventId );
    event.setEndDateTime( QDateTime::currentDateTime() );

    emit requestEventModification( event );

    if ( m_activeEventIds.isEmpty() ) m_timer.stop();
}

void CharmDataModel::endAllEventsRequested()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    while ( ! m_activeEventIds.isEmpty() ) {
        EventId eventId = m_activeEventIds.first();
        m_activeEventIds.pop_front();
        Q_FOREACH( CharmDataModelAdapterInterface* adapter, m_adapters ) {
            adapter->eventDeactivated( eventId );
        }

        Q_ASSERT( eventId != 0 );
        Event& event = findEvent( eventId );
        event.setEndDateTime( currentDateTime );

        emit requestEventModification( event );
    }

    m_timer.stop();
}

void CharmDataModel::eventUpdateTimerEvent()
{
    Q_FOREACH( EventId id, m_activeEventIds ) {
        Event& event = findEvent( id );
        event.setEndDateTime( QDateTime::currentDateTime() );

        emit requestEventModification( event );
    }
}

const EventMap& CharmDataModel::eventMap() const
{
    return m_events;
}

bool CharmDataModel::isEventActive( EventId id ) const
{
    return m_activeEventIds.contains( id );
}

int CharmDataModel::activeEventCount() const
{
    return m_activeEventIds.count();
}

EventIdList CharmDataModel::eventsThatStartInTimeFrame( const QDateTime& start,
                                                        const QDateTime& end ) const
{
    EventIdList events;
    EventMap::const_iterator it;
    for ( it = m_events.begin();
          it != m_events.end(); ++it ) {
        const Event& event( it->second );
        if ( event.startDateTime() >= start && event.startDateTime() < end ) {
            events << event.id();
        }
    }

    return events;
}

EventIdList CharmDataModel::eventsThatStartInTimeFrame( const TimeSpan& timeSpan ) const
{
    return eventsThatStartInTimeFrame(
        QDateTime( timeSpan.first ),
        QDateTime( timeSpan.second ) );
}

bool CharmDataModel::isParentOf( TaskId parent, TaskId id )
{
    Q_ASSERT_X( parent != 0, "CharmDataModel::isParentOf",
                "parent is invalid (0)" );

    if ( id == parent ) return false; // a task is not it's own child
    // get the item, make sure it is valid
    const TaskTreeItem& item( taskTreeItem( id ) );
    Q_ASSERT_X( item.isValid(), "CharmDataModel::isParentOf",
                "No such task" );
    if ( ! item.isValid() ) return false;

    TaskId parentId = item.task().parent();

    if ( parentId == parent ) return true; // found it on the path
    if ( parentId == 0 ) return false; // the task has no parent
    return isParentOf( parent, parentId );
}

EventIdList CharmDataModel::activeEvents() const
{
    return m_activeEventIds;
}

#include "CharmDataModel.moc"


