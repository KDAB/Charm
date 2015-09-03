/*
  CharmDataModel.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2007-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>
  Author: Frank Osterfeld <frank.osterfeld@kdab.com>
  Author: David Faure <david.faure@kdab.com>
  Author: Mike McQuaid <mike.mcquaid@kdab.com>
  Author: Guillermo A. Amaral <gamaral@kdab.com>
  Author: Allen Winter <allen.winter@kdab.com>

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

#include "CharmDataModel.h"
#include "CharmConstants.h"
#include "Configuration.h"

#include <QList>
#include <QtDebug>
#include <QDateTime>
#include <QSettings>
#include <QStringList>

#include <algorithm>
#include <functional>
#include <queue>

CharmDataModel::CharmDataModel()
    : QObject()
{
    connect( &m_timer, SIGNAL(timeout()), SLOT(eventUpdateTimerEvent()) );
}

CharmDataModel::~CharmDataModel()
{
    m_adapters.clear();
    setAllTasks( TaskList() );
}

void CharmDataModel::stateChanged( State previous, State next )
{
    if ( previous == Connected && next == Disconnecting ) {
        Q_FOREACH( EventId id, m_activeEventIds ) {
            const Event& event = findEvent( id );
            const Task& task = findTask( event.taskId() );
            Q_ASSERT( task.isValid() );
            endEventRequested( task );
        }
        setAllTasks( TaskList() );
    }
}

void CharmDataModel::registerAdapter( CharmDataModelAdapterInterface* adapter )
{
    m_adapters.append( adapter );
    adapter->resetEvents();
}

void CharmDataModel::unregisterAdapter( CharmDataModelAdapterInterface* adapter )
{
    Q_ASSERT( m_adapters.contains( adapter ) );
    m_adapters.removeAll( adapter );
}

void CharmDataModel::setAllTasks( const TaskList& tasks )
{
    clearTasks();

    Q_ASSERT( Task::checkForTreeness( tasks ) );
    Q_ASSERT( Task::checkForUniqueTaskIds( tasks ) );

    // fill the tasks into the map to TaskTreeItems
    for ( int i = 0; i < tasks.size(); ++i )
    {
        const TaskTreeItem item( tasks[i], &m_rootItem );
        Q_ASSERT( ! taskExists( tasks[i].id() ) ); // the tasks form a tree and have unique task ids
        m_tasks[ tasks[i].id() ] = item;
    }

    // create parent-child-relationships:
    for ( TaskTreeItem::Map::iterator it = m_tasks.begin(); it != m_tasks.end(); ++it )
    {
        const Task& task = it->second.task();
        TaskTreeItem& parent = parentItem( task );
        it->second.makeChildOf( parent );
    }

    // store task id length:
    determineTaskPaddingLength();

    m_nameCache.setAllTasks( tasks );

    // notify adapters of changes
    for_each( m_adapters.begin(), m_adapters.end(),
              std::mem_fun( &CharmDataModelAdapterInterface::resetTasks ) );

    emit resetGUIState();
}

void CharmDataModel::addTask( const Task& task )
{
    Q_ASSERT_X( ! taskExists( task.id() ), Q_FUNC_INFO,
                "New tasks need to have a unique task id" );

    if ( task.isValid() && ! taskExists( task.id() ) ) {
        const TaskTreeItem& parent = taskTreeItem( task.parent() );

        Q_FOREACH( auto adapter, m_adapters )
            adapter->taskAboutToBeAdded( parent.task().id(),
                                         parent.childCount() );

        const TaskTreeItem item ( task );
        m_tasks[ task.id() ] = item;
        m_nameCache.addTask( task );

        // the item in the map has a different address, let's find it:
        Q_ASSERT( taskExists( task.id() ) ); // we just put it in
        const auto it = m_tasks.find( task.id() );
        it->second.makeChildOf( parentItem( task ) );

        determineTaskPaddingLength();
//        regenerateSmartNames();

        Q_FOREACH( auto adapter, m_adapters )
            adapter->taskAdded( task.id() );
    } else {
        qDebug() << "CharmDataModel::addTask: duplicate task id"
                 << task.id() << "ignored. THIS IS A BUG";
    }
}

void CharmDataModel::modifyTask( const Task& task )
{
    const auto it = m_tasks.find( task.id() );
    Q_ASSERT_X( it != m_tasks.end(), Q_FUNC_INFO,
              "Task to modify has to exist" );

    if ( it == m_tasks.end() )
        return;
    const TaskId oldParentId = it->second.task().parent();
    const bool parentChanged = task.parent() != oldParentId;

    if ( parentChanged ) {
        Q_FOREACH( auto adapter, m_adapters )
            adapter->taskParentChanged( task.id(), oldParentId, task.parent() );
        m_tasks[ task.id() ].makeChildOf( parentItem( task ) );
    }

    m_tasks[ task.id() ].task() = task;
    m_nameCache.modifyTask( task );

    if( parentChanged ) {
        Q_FOREACH( auto adapter, m_adapters )
            adapter->resetTasks();
    } else {
        Q_FOREACH( auto adapter, m_adapters )
            adapter->taskModified( task.id() );
    }
}

void CharmDataModel::deleteTask( const Task& task )
{
    Q_ASSERT_X( taskExists( task.id() ), Q_FUNC_INFO,
                "Task to delete has to exist" );
    Q_ASSERT_X( taskTreeItem( task.id() ).childCount() == 0,
                Q_FUNC_INFO,
                "Cannot delete a task that has children" );

    Q_FOREACH( auto adapter, m_adapters )
        adapter->taskAboutToBeDeleted( task.id() );

    const auto it = m_tasks.find( task.id() );
    if ( it != m_tasks.end() ) {
        TaskTreeItem tmpParent;
        it->second.makeChildOf( tmpParent );
        m_tasks.erase( it );
    }

    m_nameCache.deleteTask( task );

    Q_FOREACH( auto adapter, m_adapters )
        adapter->taskDeleted( task.id() );
}

void CharmDataModel::clearTasks()
{
    // to clear the task list, all tasks have first to be changed to be children of the root item:
    for ( TaskTreeItem::Map::iterator it = m_tasks.begin(); it != m_tasks.end(); ++it )
    {
        it->second.makeChildOf( m_rootItem );
    }

    m_tasks.clear();
    m_nameCache.clearTasks();
    m_rootItem = TaskTreeItem();

    Q_FOREACH( auto adapter, m_adapters )
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

    Q_FOREACH( auto adapter, m_adapters )
        adapter->resetEvents();
}

void CharmDataModel::addEvent( const Event& event )
{
    Q_ASSERT_X( ! eventExists( event.id() ), Q_FUNC_INFO,
                "New event must have a unique id" );

    Q_FOREACH( auto adapter, m_adapters )
        adapter->eventAboutToBeAdded( event.id() );

    m_events[ event.id() ] = event;

    Q_FOREACH( auto adapter, m_adapters )
        adapter->eventAdded( event.id() );
}

void CharmDataModel::modifyEvent( const Event& newEvent )
{
    Q_ASSERT_X( eventExists( newEvent.id() ), Q_FUNC_INFO,
                "Event to modify has to exist" );

    const Event oldEvent = eventForId( newEvent.id() );

    m_events[ newEvent.id() ] = newEvent;

    Q_FOREACH( auto adapter, m_adapters )
        adapter->eventModified( newEvent.id(), oldEvent );
}

void CharmDataModel::deleteEvent( const Event& event )
{
    Q_ASSERT_X( eventExists( event.id() ), Q_FUNC_INFO,
                "Event to delete has to exist" );
    Q_ASSERT_X( !m_activeEventIds.contains( event.id() ), Q_FUNC_INFO,
                "Cannot delete an active event" );

    Q_FOREACH( auto adapter, m_adapters )
        adapter->eventAboutToBeDeleted( event.id() );

    const auto it = m_events.find( event.id() );
    if ( it != m_events.end() )
        m_events.erase( it );

    Q_FOREACH( auto adapter, m_adapters )
        adapter->eventDeleted( event.id() );
}

void CharmDataModel::clearEvents()
{
    m_events.clear();

    Q_FOREACH( auto adapter, m_adapters )
        adapter->resetEvents();
}

const TaskTreeItem& CharmDataModel::taskTreeItem( TaskId id ) const
{
    if ( id <= 0 ) return m_rootItem;

    const auto it = m_tasks.find( id );
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
    const TaskTreeItem::Map::iterator it = m_tasks.find( id );
    Q_ASSERT( it != m_tasks.end() );
    return it->second.task();
}

const Event& CharmDataModel::eventForId( EventId id ) const
{
    static const Event InvalidEvent;
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
    const auto it = m_events.find( id );
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
    Q_FOREACH( auto adapter, m_adapters ) {
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
    TaskTreeItem& parent = m_tasks[ task.parent() ];
    if ( parent.isValid() ) {
        return parent;
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
    if ( !m_activeEventIds.isEmpty() ) {
        endAllEventsRequested();
    }

    // clear the "last event editor datetime" so that the next manual "create event"
    // doesn't use some old date
    QSettings settings;
    settings.remove( MetaKey_LastEventEditorDateTime );

    emit makeAndActivateEvent( task );
    updateToolTip();
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
            Q_FOREACH( auto adapter, m_adapters ) {
                adapter->eventDeactivated( eventId );
            }
            break;
        }
    }

    Q_ASSERT( eventId != 0 );
    Event& event = findEvent( eventId );
    Event old = event;
    event.setEndDateTime( QDateTime::currentDateTime() );

    emit requestEventModification( event, old );

    if ( m_activeEventIds.isEmpty() ) m_timer.stop();
    updateToolTip();
}

void CharmDataModel::endAllEventsRequested()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    while ( ! m_activeEventIds.isEmpty() ) {
        EventId eventId = m_activeEventIds.first();
        m_activeEventIds.pop_front();
        Q_FOREACH( auto adapter, m_adapters ) {
            adapter->eventDeactivated( eventId );
        }

        Q_ASSERT( eventId != 0 );
        Event& event = findEvent( eventId );
        Event old = event;
        event.setEndDateTime( currentDateTime );

        emit requestEventModification( event, old );
    }

    m_timer.stop();
    updateToolTip();
}

void CharmDataModel::eventUpdateTimerEvent()
{
    Q_FOREACH( EventId id, m_activeEventIds ) {
        // Not a ref (Event &), since we want to diff "old event"
        // and "new event" in *Adapter::eventModified
        Event event = findEvent( id );
        Event old = event;
        event.setEndDateTime( QDateTime::currentDateTime() );

        emit requestEventModification( event, old );
    }
    updateToolTip();
}

QString CharmDataModel::fullTaskName( const Task& task ) const
{
    if ( task.isValid() ) {
        QString name = task.name().simplified();

        if ( task.parent() != 0 ) {
            const Task& parent = getTask( task.parent() );
            if ( parent.isValid() ) {
                name = fullTaskName( parent ) + '/' + name;
            }
        }
        return name;
    } else {
        // qWarning() << "CharmReport::tasknameWithParents: WARNING: invalid task"
        //                    << task.id();
        return QString();
    }
}

QString CharmDataModel::smartTaskName( const Task & task ) const
{
    return m_nameCache.smartName( task.id() );
}

QString CharmDataModel::eventsString() const
{
    QStringList eStrList;
    Q_FOREACH ( EventId eventId, activeEvents() ) {
        Event event = eventForId( eventId );
        if ( event.isValid() ) {
            const Task& task = getTask( event.taskId() );
            const int taskIdLength = CONFIGURATION.taskPaddingLength;
            eStrList <<
                tr( "%1 - %2 %3" )
                .arg( hoursAndMinutes( event.duration() ) )
                .arg( task.id(), taskIdLength, 10, QChar( '0' ) )
                .arg( fullTaskName( task ) );
        }
    }
    return eStrList.join( "\n" );
}

QString CharmDataModel::taskIdAndFullNameString(TaskId id) const
{
    return QString("%1 %2")
            .arg( id, CONFIGURATION.taskPaddingLength, 10, QChar( '0' ) )
            .arg( fullTaskName( getTask( id ) ) );
}

QString CharmDataModel::taskIdAndSmartNameString(TaskId id) const
{
    return QString("%1 %2")
            .arg( id, CONFIGURATION.taskPaddingLength, 10, QChar( '0' ) )
            .arg( smartTaskName( getTask( id ) ) );
}


QString CharmDataModel::taskIdAndNameString(TaskId id) const
{
    return QString("%1 %2")
            .arg( id, CONFIGURATION.taskPaddingLength, 10, QChar( '0' ) )
            .arg( getTask( id ).name() );
}

int CharmDataModel::totalDuration() const
{
    int totalDuration = 0;
    Q_FOREACH ( EventId eventId, activeEvents() ) {
        Event event = eventForId( eventId );
        if ( event.isValid() ) {
            totalDuration += event.duration();
        }
    }
    return totalDuration;
}

QString CharmDataModel::totalDurationString() const
{
    return hoursAndMinutes( totalDuration() );
}

void CharmDataModel::updateToolTip()
{
    QString toolTip;
    int numEvents = activeEvents().count();
    switch( numEvents ) {
    case 0:
        toolTip = tr( "No active events" );
        break;
    case 1:
        toolTip = eventsString();
        break;
    default:
        toolTip = tr( "<qt>%1 for %2 active events:<hr>%3</qt>" )
                  .arg( totalDurationString() ).arg( numEvents ).arg( eventsString() );
        break;
    }

    emit sysTrayUpdate( toolTip, numEvents != 0 );
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

EventIdList CharmDataModel::eventsThatStartInTimeFrame( const QDate& start,
                                                        const QDate& end ) const
{
    EventIdList events;
    EventMap::const_iterator it;
    for ( it = m_events.begin();
          it != m_events.end(); ++it ) {
        const Event& event( it->second );
        if ( event.startDateTime().date() >= start && event.startDateTime().date() < end ) {
            events << event.id();
        }
    }

    return events;
}

EventIdList CharmDataModel::eventsThatStartInTimeFrame( const TimeSpan& timeSpan ) const
{
    return eventsThatStartInTimeFrame( timeSpan.first, timeSpan.second );
}

bool CharmDataModel::isParentOf( TaskId parent, TaskId id ) const
{
    Q_ASSERT_X( parent != 0, Q_FUNC_INFO, "parent is invalid (0)" );

    if ( id == parent ) return false; // a task is not it's own child
    // get the item, make sure it is valid
    const TaskTreeItem& item( taskTreeItem( id ) );
    Q_ASSERT_X( item.isValid(), Q_FUNC_INFO, "No such task" );
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

struct TaskWithCount {
    TaskId id;
    unsigned int count;

    bool operator<( const TaskWithCount& other ) const {
        return count < other.count;
    }
};

struct TaskWithLastUseDate {
    TaskId id;
    QDateTime lastUse;

    bool operator<( const TaskWithLastUseDate& other ) const {
        return lastUse < other.lastUse;
    }
};

TaskIdList CharmDataModel::mostFrequentlyUsedTasks() const
{
    QMap<TaskId, unsigned > mfuMap;
    const EventMap& events = eventMap();
    for( EventMap::const_iterator it = events.begin(); it != events.end(); ++it ) {
        const TaskId id = it->second.taskId();
        // process use count
        const unsigned count  = mfuMap[id] + 1;
        mfuMap[id] = count;
    }
    std::priority_queue<TaskWithCount> mfuTasks;
    for( QMap<TaskId, unsigned >::const_iterator it = mfuMap.constBegin(); it != mfuMap.constEnd(); ++it ) {
        TaskWithCount t;
        t.id = it.key();
        t.count = it.value();
        mfuTasks.push( t );
    }
    TaskIdList mfu;
    while( ! mfuTasks.empty() ) {
        const TaskWithCount t = mfuTasks.top();
        mfuTasks.pop();
        mfu.append( t.id );
    }
    return mfu;
}

TaskIdList CharmDataModel::mostRecentlyUsedTasks() const
{
    QMap<TaskId, QDateTime> mruMap;
    const EventMap& events = eventMap();
    for( EventMap::const_iterator it = events.begin(); it != events.end(); ++it ) {
        const TaskId id = it->second.taskId();
        // process use date
        const QDateTime date = it->second.startDateTime();
        mruMap[id]= qMax( mruMap[id], date );
    }
    std::priority_queue<TaskWithLastUseDate> mruTasks;
    for( QMap<TaskId, QDateTime>::const_iterator it = mruMap.constBegin(); it != mruMap.constEnd(); ++it ) {
        TaskWithLastUseDate t;
        t.id = it.key();
        t.lastUse = it.value();
        if( t.id != 0 ) mruTasks.push( t );
    }
    TaskIdList mru;
    while( ! mruTasks.empty() ) {
        const TaskWithLastUseDate t = mruTasks.top();
        mruTasks.pop();
        Q_ASSERT( t.id != 0 );
        mru.append( t.id );
    }

    return mru;
}

bool CharmDataModel::operator==( const CharmDataModel& other ) const
{
    // not compared: m_timer, m_adapters
    if( &other == this ) {
        return true;
    }
    return getAllTasks() == other.getAllTasks()
            && m_events == other.m_events
            && m_activeEventIds == other.m_activeEventIds;
}

CharmDataModel* CharmDataModel::clone() const
{
    auto c = new CharmDataModel();
    c->setAllTasks( getAllTasks() );
    c->m_events = m_events;
    c->m_activeEventIds = m_activeEventIds;
    return c;
}

#include "moc_CharmDataModel.cpp"
