#ifndef TASK_H
#define TASK_H

#include <map>

#include <QList>
#include <QString>
#include <QtDebug>
#include <QMetaType>
#include <QDomElement>
#include <QDomDocument>
#include <QDateTime>

#include "CharmExceptions.h"

typedef int TaskId;
Q_DECLARE_METATYPE( TaskId )

class Task;
/** A task list is a list of tasks that belong together.
    Example: All tasks for one user. */
typedef QList<Task> TaskList;
typedef QList<TaskId> TaskIdList;

/** A task is a category under which events are filed.
    It has a unique identifier and a name. */
class Task {
public:
    Task();
    /** Convenience constructor. */
    Task( TaskId id, const QString& name, TaskId parent = 0, bool subscribed = false );

    bool isValid() const;

    bool operator == ( const Task& other ) const;
    bool operator != ( const Task& other ) const { return ! operator==( other ); }

    TaskId id() const ;

    void setId( TaskId id );

    QString name() const;

    void setName( const QString& name );

    TaskId parent() const;

    void setParent( TaskId parent );

    bool subscribed() const;

    void setSubscribed( bool value );

    const QDateTime& validFrom() const;

    void setValidFrom( const QDateTime& );

    const QDateTime& validUntil() const;

    void setValidUntil( const QDateTime& );

    bool isCurrentlyValid() const;

    void dump() const;

    static QString tagName();
    static QString taskListTagName();

    QDomElement toXml( QDomDocument ) const;

    static Task fromXml( const QDomElement&, int databaseSchemaVersion = 1 );

    static TaskList readTasksElement( const QDomElement&, int databaseSchemaVersion = 1 );

    static QDomElement makeTasksElement( QDomDocument, const TaskList& );

    static bool checkForUniqueTaskIds( TaskList tasks );

    static bool checkForTreeness( const TaskList& tasks );

    static bool lowerTaskId( const Task& left, const Task& right );

private:
    int m_id;
    int m_parent;
    QString m_name;
    bool m_subscribed;
    /** The timestamp from which the task is valid. */
    QDateTime m_validFrom;
    /** The timestamp after which the task becomes invalid. */
    QDateTime m_validUntil;
};

Q_DECLARE_METATYPE( TaskIdList )
Q_DECLARE_METATYPE( TaskList )

void dumpTaskList( const TaskList& tasks );

#endif
