#include <set>
#include <algorithm>

#include <QtDebug>

#include "Task.h"
#include "CharmConstants.h"
#include "CharmExceptions.h"

Task::Task() :
	m_id(0), m_parent(0), m_subscribed(false)
{
}

Task::Task(TaskId id, const QString& name, TaskId parent, bool subscribed) :
	m_id(id), m_parent(parent), m_name(name), m_subscribed(subscribed)
{
}

bool Task::isValid() const
{
	return id() != 0;
}

QString Task::tagName()
{
    static const QString tag( QString::fromAscii( "task" ) );
    return tag;
}

QString Task::taskListTagName()
{
    static const QString tag( QString::fromAscii( "tasks" ) );
    return tag;
}

bool Task::operator == ( const Task& other ) const
{
    return
            other.id() == id()
            && other.parent() == parent()
            && other.name() == name()
            && other.subscribed() == subscribed()
            && other.validFrom() == validFrom()
            && other.validUntil() == validUntil();
}

TaskId Task::id() const
{
	return m_id;
}

void Task::setId(TaskId id)
{
	m_id = id;
}

QString Task::name() const
{
	return m_name;
}

void Task::setName(const QString& name)
{
	m_name = name;
}

int Task::parent() const
{
	return m_parent;
}

void Task::setParent(int parent)
{
	m_parent = parent;
}

bool Task::subscribed() const
{
	return m_subscribed;
}

void Task::setSubscribed(bool value)
{
	m_subscribed = value;
}

const QDateTime& Task::validFrom() const
{
	return m_validFrom;
}

void Task::setValidFrom(const QDateTime& stamp)
{
	m_validFrom = stamp;
	QTime time( m_validFrom.time() );
	time.setHMS( time.hour(), time.minute(), time.second() );
	m_validFrom.setTime( time );
}

const QDateTime& Task::validUntil() const
{
	return m_validUntil;
}

void Task::setValidUntil(const QDateTime& stamp)
{
	m_validUntil = stamp;
	QTime time( m_validUntil.time() );
	time.setHMS( time.hour(), time.minute(), time.second() );
	m_validUntil.setTime( time );
}

bool Task::isCurrentlyValid() const
{
	return isValid()
		&& ( ! validFrom().isValid() || validFrom() < QDateTime::currentDateTime() )
		&& ( ! validUntil().isValid() || validUntil() > QDateTime::currentDateTime() );
}

void Task::dump() const
{
	qDebug() << "[Task " << this << "] task id:" << id() << "- name:" << name()
			<< " - parent:" << parent() << " - subscribed:" << subscribed()
			<< " - valid from:" << validFrom() << " - valid until:"
			<< validUntil();
}

void dumpTaskList(const TaskList& tasks)
{
	qDebug() << "dumpTaskList: task list of" << tasks.size() << "elements";
	for (int i = 0; i < tasks.size(); ++i)
	{
		tasks[i].dump();
	}
}

// FIXME make XmlSerializable interface, with tagName/toXml/fromXml:
const QString TaskIdElement("taskid");
const QString TaskParentId("parentid");
const QString TaskSubscribed("subscribed");
const QString TaskValidFrom("validfrom");
const QString TaskValidUntil("validuntil");

QDomElement Task::toXml(QDomDocument document) const
{
	QDomElement element = document.createElement( tagName() );
	element.setAttribute(TaskIdElement, id());
	element.setAttribute(TaskParentId, parent());
	element.setAttribute(TaskSubscribed, (subscribed() ? 1 : 0));
	if (!name().isEmpty())
	{
		QDomText taskName = document.createTextNode(name());
		element.appendChild(taskName);
	}
	if (validFrom().isValid())
	{
		element.setAttribute(TaskValidFrom, validFrom().toString(Qt::ISODate));
	}
	if (validUntil().isValid())
	{
		element.setAttribute(TaskValidUntil, validUntil().toString(Qt::ISODate));
	}
	return element;
}

Task Task::fromXml(const QDomElement& element, int databaseSchemaVersion)
{   // in case any task object creates trouble with
    // serialization/deserialization, add an object of it to
    // void XmlSerializationTests::testTaskSerialization()
    if ( element.tagName() != tagName() ) {
        throw XmlSerializationException( QObject::tr( "Task::fromXml: judging from the tag name, this is not a task tag") );
    }

    Task task;
    bool ok;
    task.setName(element.text());
    task.setId(element.attribute(TaskIdElement).toInt(&ok));
    if (!ok)
        throw XmlSerializationException( QObject::tr( "Task::fromXml: invalid task id") );
    task.setParent(element.attribute(TaskParentId).toInt(&ok));
    if (!ok)
        throw XmlSerializationException( QObject::tr( "Task::fromXml: invalid parent task id") );
    task.setSubscribed(element.attribute(TaskSubscribed).toInt(&ok) == 1);
    if (!ok)
        throw XmlSerializationException( QObject::tr( "Task::fromXml: invalid subscription setting") );
    if( databaseSchemaVersion > CHARM_DATABASE_VERSION_BEFORE_TASK_EXPIRY ) {
        if ( element.hasAttribute( TaskValidFrom ) ) {
            QDateTime start = QDateTime::fromString( element.attribute( TaskValidFrom ), Qt::ISODate );
            if ( !start.isValid() ) throw XmlSerializationException( QObject::tr( "Task::fromXml: invalid valid-from date" ) );
            task.setValidFrom( start );
        }
        if ( element.hasAttribute( TaskValidUntil ) ) {
            QDateTime end = QDateTime::fromString( element.attribute( TaskValidUntil ), Qt::ISODate );
            if ( !end.isValid() ) throw XmlSerializationException( QObject::tr( "Task::fromXml: invalid valid-until date" ) );
            task.setValidUntil( end );
        }
    }
    return task;
}

TaskList Task::readTasksElement( const QDomElement& element, int databaseSchemaVersion )
{
    if ( element.tagName() == taskListTagName() ) {
        TaskList tasks;
	for ( QDomElement child = element.firstChildElement(); !child.isNull();
              child = child.nextSiblingElement( tagName() ) ) {
            if ( child.tagName() != tagName() ) {
                throw XmlSerializationException( QObject::tr( "Task::readTasksElement: parent-child mismatch" ) );
            }
            Task task = fromXml( child, databaseSchemaVersion );
            tasks << task;
	}
        return tasks;
    } else {
        throw XmlSerializationException( QObject::tr( "Task::readTasksElement: judging by the tag name, this is not a tasks element" ) );
    }
}


QDomElement Task::makeTasksElement( QDomDocument document, const TaskList& tasks )
{
    QDomElement element = document.createElement( taskListTagName() );
    Q_FOREACH( const Task& task, tasks ) {
        element.appendChild( task.toXml( document ) );
    }
    return element;
}

bool Task::lowerTaskId( const Task& left,  const Task& right )
{
    return left.id() < right.id();
}

bool Task::checkForUniqueTaskIds( TaskList tasks )
{
    std::set<TaskId> ids;
    for ( TaskList::const_iterator it = tasks.begin(); it != tasks.end(); ++it ) {
        if ( ids.find( ( *it ).id() ) != ids.end() ) {
            return false;
        }
        ids.insert( ( *it ).id() );
    }
    return true;
}

/** collectTaskIds visits the task and all subtasks recursively, and
 * adds all visited task ids to visitedIds.
 * @returns false if any visited task id is already in visitedIds
 * @param id the parent task to traverse
 * @param visitedIds reference to a TaskId set that contains already
 * visited task ids
 * @param tasks the tasklist to process
 */
bool collectTaskIds( std::set<TaskId>& visitedIds, TaskId id, const TaskList& tasks )
{
    bool foundSelf = false;
    TaskIdList children;

    // find children and the task itself (the parameter tasks is not sorted)
    for ( TaskList::const_iterator it = tasks.begin(); it != tasks.end(); ++it ) {
        if ( ( *it ).parent() == id ) {
            children << ( *it ).id();
        }
        if ( ( *it ).id() == id ) {
            // just checking that it really exists...
            if ( std::find( visitedIds.begin(), visitedIds.end(), id ) != visitedIds.end() ) {
                return false;
            } else {
                if ( ( *it ).isValid() ) {
                    visitedIds.insert( id );
                    foundSelf = true;
                } else {
                    return false;
                }
            }
        }
    }

    if ( !foundSelf ) {
        return false;
    }

    Q_FOREACH( TaskId id, children ) {
        collectTaskIds( visitedIds, id, tasks );
    }

    return true;
}


/** checkForTreeness checks a task list against cycles in the
 * parent-child relationship, and for orphans (tasks where the parent
 * task does not exist). If the task list contains invalid tasks,
 * false is returned as well.
 *
 * @return false, if cycles in the task tree or orphans have been found
 * @param tasks the tasklist to verify
 */
bool Task::checkForTreeness( const TaskList& tasks )
{
    std::set<TaskId> ids;

    for ( TaskList::const_iterator it = tasks.begin(); it != tasks.end(); ++it ) {
        if ( ! ( *it ).isValid() ) {
            return false;
        }
        if ( ( *it ).parent() == 0 ) {
            if ( ! collectTaskIds( ids, ( *it ).id(), tasks ) ) {
                return false;
            }
        }
    }

    // the count of ids now must be equal to the count of tasks,
    // otherwise tasks contains elements that are not in the subtrees
    // of toplevel elements
    if ( ids.size() != static_cast<unsigned>( tasks.size() ) ) {
#if 0
        Q_FOREACH( const Task& task, tasks ) {
            if ( find( ids.begin(), ids.end(), task.id() ) == ids.end() ) {
                qDebug() << "Orphan task:";
                task.dump();
            }
        }
#endif
        return false;
    }

    return true;
}


