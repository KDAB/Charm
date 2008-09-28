#include <QtDebug>

#include "Task.h"
#include "CharmConstants.h"

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
	return ( ! validFrom().isValid() || validFrom() < QDateTime::currentDateTime() )
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
    throw(XmlSerializationException )
{   // in case any task object creates trouble with
    // serialization/deserialization, add an object of it to
    // void XmlSerializationTests::testTaskSerialization()
    if ( element.tagName() != tagName() ) {
        throw XmlSerializationException("Task::fromXml: judging from the tag name, this is not a task tag");
    }

    Task task;
    bool ok;
    task.setName(element.text());
    task.setId(element.attribute(TaskIdElement).toInt(&ok));
    if (!ok)
        throw XmlSerializationException("Task::fromXml: invalid task id");
    task.setParent(element.attribute(TaskParentId).toInt(&ok));
    if (!ok)
        throw XmlSerializationException("Task::fromXml: invalid parent task id");
    task.setSubscribed(element.attribute(TaskSubscribed).toInt(&ok) == 1);
    if (!ok)
        throw XmlSerializationException(
            "Task::fromXml: invalid subscription setting");
    if( databaseSchemaVersion > CHARM_DATABASE_VERSION_BEFORE_TASK_EXPIRY ) {
        if ( element.hasAttribute( TaskValidFrom ) ) {
            QDateTime start = QDateTime::fromString( element.attribute( TaskValidFrom ), Qt::ISODate );
            if ( !start.isValid() ) throw XmlSerializationException( "Task::fromXml: invalid valid-from date" );
            task.setValidFrom( start );
        }
        if ( element.hasAttribute( TaskValidUntil ) ) {
            QDateTime end = QDateTime::fromString( element.attribute( TaskValidUntil ), Qt::ISODate );
            if ( !end.isValid() ) throw XmlSerializationException( "Task::fromXml: invalid valid-until date" );
            task.setValidUntil( end );
        }
    }
    return task;
}

TaskList Task::readTasksElement( const QDomElement& element, int databaseSchemaVersion ) throw( XmlSerializationException )
{
    if ( element.tagName() == taskListTagName() ) {
        TaskList tasks;
	for ( QDomElement child = element.firstChildElement(); !child.isNull();
              child = child.nextSiblingElement( tagName() ) ) {
            if ( child.tagName() != tagName() ) {
                throw XmlSerializationException( "Task::readTasksElement: parent-child mismatch" );
            }
            Task task = fromXml( child, databaseSchemaVersion );
            tasks << task;
	}
        return tasks;
    } else {
        throw XmlSerializationException( "Task::readTasksElement: judging by the tag name, this is not a tasks element" );
    }
}


QDomElement Task::makeTasksElement( QDomDocument document, const TaskList& tasks ) throw( XmlSerializationException )
{
    QDomElement element = document.createElement( taskListTagName() );
    Q_FOREACH( const Task& task, tasks ) {
        element.appendChild( task.toXml( document ) );
    }
    return element;
}

