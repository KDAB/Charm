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
     * More precisely, all events that start at or after @p start, and start before @p end
     */
    EventIdList eventsThatStartInTimeFrame( const QDateTime& start,
                                            const QDateTime& end ) const;
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

    bool operator==( const CharmDataModel& other ) const;

signals:
    // these need to be implemented in the respective application to
    // be able to track time:
    void makeAndActivateEvent( const Task& );
    void requestEventModification( const Event& );
    void sysTrayUpdate( const QString&, bool );

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

private slots:
    void eventUpdateTimerEvent();

private:
    // functions only used for testing:
    CharmDataModel* clone() const;
};
#endif
