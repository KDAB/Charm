/*
  CharmDataModel.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2007-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>
  Author: Frank Osterfeld <frank.osterfeld@kdab.com>
  Author: Allen Winter <allen.winter@kdab.com>
  Author: David Faure <david.faure@kdab.com>

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

#ifndef CHARMDATAMODEL_H
#define CHARMDATAMODEL_H

#include <QObject>
#include <QTimer>

#include "Task.h"
#include "State.h"
#include "Event.h"
#include "TimeSpans.h"
#include "TaskTreeItem.h"
#include "CharmDataModelAdapterInterface.h"
#include "SmartNameCache.h"

class QAbstractItemModel;

/** CharmDataModel is the application's model.
    CharmDataModel holds all data that makes up the application's
    current data space: the list of tasks, the list of events, and the
    list of active (currently timed) events.
    It will notify all registered CharmDataModelAdapterInterfaces
    about changes in the model. Those interfaces could, for example,
    implement QAbstractItemModel.
*/
class CharmDataModel : public QObject
{
    Q_OBJECT
    friend class ImportExportTests;

public:
    CharmDataModel();
    ~CharmDataModel();

    void stateChanged( State previous, State next );
    /** Register a CharmDataModelAdapterInterface. */
    void registerAdapter( CharmDataModelAdapterInterface* );
    /** Unregister a CharmDataModelAdapterInterface. */
    void unregisterAdapter( CharmDataModelAdapterInterface* );

    /** Retrieve a task for the given task id.
        If called with Zero as the task id, it will return the
        imaginary root that has all top-levels as it's children.
    */
    const TaskTreeItem& taskTreeItem( TaskId id ) const;
    /** Convenience method: retrieve the task directly. */
    const Task& getTask( TaskId id ) const;
    /** Get all tasks as a TaskList.
        Warning: this might be slow. */
    TaskList getAllTasks() const;
    /** Retrieve an event for the given event id. */
    const Event& eventForId( EventId id ) const;
    /** Constant access to the map of events. */
    const EventMap& eventMap() const;
    /**
     * Get all events that start in a given time frame (e.g. a given day, a given week etc.)
     * More precisely, all events that start at or after @p start, and start before @p end (@p end excluded!)
     */
    EventIdList eventsThatStartInTimeFrame( const QDate& start,
                                            const QDate& end ) const;
    // convenience overload
    EventIdList eventsThatStartInTimeFrame( const TimeSpan& timeSpan ) const;
    const Event& activeEventFor ( TaskId id ) const;
    EventIdList activeEvents() const;
    int activeEventCount() const;
    TaskTreeItem& parentItem( const Task& task ); // FIXME const???
    bool taskExists( TaskId id );
    /** True if task is in the subtree below parent.
     * parent is not element of the subtree, and thus not it's own child. */
    bool isParentOf( TaskId parent, TaskId task ) const;

    // handling of active events:
    /** Is an event active for the task with this id? */
    bool isTaskActive( TaskId id ) const;
    /** Is this event active? */
    bool isEventActive( EventId id ) const;
    /** Start a new event with this task. */
    void startEventRequested( const Task& );
    /** Stop the active event for this task. */
    void endEventRequested( const Task& );
    /** Stop all tasks. */
    void endAllEventsRequested();
    /** Activate this event. */
    bool activateEvent( const Event& );

    /** Provide a list of the most frequently used tasks.
      * Only tasks that have been used so far will be taken into account, so the list might be empty. */
    TaskIdList mostFrequentlyUsedTasks() const;
    /** Provide a list of the most recently used tasks.
      * Only tasks that have been used so far will be taken into account, so the list might be empty. */
    TaskIdList mostRecentlyUsedTasks() const;

    /** Create a full task name from the specified TaskId. */
    QString fullTaskName( const Task& ) const;

    /** Create a "smart" task name (name and shortest path that makes the name unique) from the specified TaskId. */
    QString smartTaskName( const Task& ) const;

    /** Get the task id and full name as a single string. */
    QString taskIdAndFullNameString(TaskId id) const;

    /** Get the task id and name as a single string. */
    QString taskIdAndNameString(TaskId id) const;

    /** Get the task id and smart name as a single string. */
    QString taskIdAndSmartNameString(TaskId id) const;

    bool operator==( const CharmDataModel& other ) const;

signals:
    // these need to be implemented in the respective application to
    // be able to track time:
    void makeAndActivateEvent( const Task& );
    void requestEventModification( const Event&, const Event& );
    void sysTrayUpdate( const QString&, bool );
    void resetGUIState();

public slots:
    void setAllTasks( const TaskList& tasks );
    void addTask( const Task& );
    void modifyTask( const Task& );
    void deleteTask( const Task& );
    void clearTasks();

    void setAllEvents( const EventList& events );
    void addEvent( const Event& );
    void modifyEvent( const Event& );
    void deleteEvent( const Event& );
    void clearEvents();

private:
    void determineTaskPaddingLength();
    bool eventExists( EventId id );

    Task& findTask( TaskId id );
    Event& findEvent( EventId id );

    int totalDuration() const;
    QString eventsString() const;
    QString totalDurationString() const;
    void updateToolTip();

    TaskTreeItem::Map m_tasks;
    TaskTreeItem m_rootItem;

    EventMap m_events;
    EventIdList m_activeEventIds;
    // adapters are notified when the model changes
    CharmDataModelAdapterList m_adapters;

    // event update timer:
    QTimer m_timer;
    SmartNameCache m_nameCache;

private slots:
    void eventUpdateTimerEvent();

private:
    // functions only used for testing:
    CharmDataModel* clone() const;
};
#endif
