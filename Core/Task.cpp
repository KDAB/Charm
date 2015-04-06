/*
  Task.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2007-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>
  Author: Frank Osterfeld <frank.osterfeld@kdab.com>
  Author: Mike McQuaid <mike.mcquaid@kdab.com>

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

#include "Task.h"
#include "CharmConstants.h"
#include "CharmExceptions.h"

#include <QtDebug>

#include <set>
#include <algorithm>

Task::Task()
    : m_id(0)
    , m_parent(0)
    , m_subscribed(false)
    , m_trackable(true)
{
}

Task::Task(TaskId id, const QString& name, TaskId parent, bool subscribed)
    : m_id(id)
    , m_parent(parent)
    , m_name(name)
    , m_subscribed(subscribed)
    , m_trackable(true)
{
}

bool Task::isValid() const
{
    return id() != 0;
}

QString Task::tagName()
{
    static const QString tag( QString::fromLatin1( "task" ) );
    return tag;
}

QString Task::taskListTagName()
{
    static const QString tag( QString::fromLatin1( "tasks" ) );
    return tag;
}

bool Task::operator == ( const Task& other ) const
{
    return other.id() == id()
        && other.parent() == parent()
        && other.name() == name()
        && other.subscribed() == subscribed()
        && other.m_trackable == m_trackable
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

bool Task::trackable() const
{
    return m_trackable;
}

void Task::setTrackable(bool trackable)
{
    m_trackable = trackable;
}

QDateTime Task::validFrom() const
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

QDateTime Task::validUntil() const
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
             << validUntil() << " - trackable:" << trackable();
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
const QString TaskTrackable("trackable");
const QString TaskValidFrom("validfrom");
const QString TaskValidUntil("validuntil");

QDomElement Task::toXml(QDomDocument document) const
{
    QDomElement element = document.createElement( tagName() );
    element.setAttribute(TaskIdElement, id());
    element.setAttribute(TaskParentId, parent());
    element.setAttribute(TaskSubscribed, (subscribed() ? 1 : 0));
        element.setAttribute(TaskTrackable, (trackable() ? 1 : 0));
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
    if ( databaseSchemaVersion > CHARM_DATABASE_VERSION_BEFORE_TRACKABLE ) {
        task.setTrackable(element.attribute(TaskTrackable, QLatin1String("1")).toInt(&ok) == 1);
        if (!ok)
            throw XmlSerializationException( QObject::tr( "Task::fromXml: invalid trackable settings") );
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

bool Task::checkForUniqueTaskIds( const TaskList& tasks )
{
    std::set<TaskId> ids;

    for ( TaskList::const_iterator it = tasks.begin(); it != tasks.end(); ++it ) {
        ids.insert( ( *it ).id() );
    }

    return static_cast<int>(ids.size()) == tasks.size();
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

    Q_FOREACH( const TaskId i, children ) {
        collectTaskIds( visitedIds, i, tasks );
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
#ifndef NDEBUG
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
