/*
  SmartNameCache.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2012-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Frank Osterfeld <frank.osterfeld@kdab.com>

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

#include "SmartNameCache.h"



struct IdLessThan
{
    bool operator()( const Task& lhs, const Task& rhs ) const
    {
        return lhs.id() < rhs.id();
    }
};

void SmartNameCache::setAllTasks( const TaskList& taskList )
{
    m_tasks = taskList;
    sortTasks();
    regenerateSmartNames();
}

void SmartNameCache::sortTasks()
{
    qSort( m_tasks.begin(), m_tasks.end(), IdLessThan() );
}

//single add/modify/delete rebuild the cache each time.
//If necessary, this could be optimized by keeping the tasks in a "inverse" tree
//with the task names below the root and their parents further down

void SmartNameCache::modifyTask( const Task& task )
{
    const TaskList::Iterator it = qBinaryFind( m_tasks.begin(), m_tasks.end(), Task( task.id(), QString() ), IdLessThan() );
    if ( it != m_tasks.end() )
        *it = task;
    sortTasks();
    regenerateSmartNames();
}

void SmartNameCache::deleteTask( const Task& task )
{
    const TaskList::Iterator it = qBinaryFind( m_tasks.begin(), m_tasks.end(), Task( task.id(), QString() ), IdLessThan() );
    if ( it != m_tasks.end() ) {
        m_tasks.erase( it );
        regenerateSmartNames();
    }
}

void SmartNameCache::clearTasks()
{
    m_tasks.clear();
    m_smartTaskNamesById.clear();
}

Task SmartNameCache::findTask( TaskId id ) const
{
    const TaskList::ConstIterator it = qBinaryFind( m_tasks.begin(), m_tasks.end(), Task( id, QString() ), IdLessThan() );
    if ( it != m_tasks.constEnd() )
        return *it;
    else
        return Task();
}

void SmartNameCache::addTask( const Task& task )
{
    m_tasks.append( task );
    sortTasks();
    regenerateSmartNames();
}

QString SmartNameCache::smartName( const TaskId& id ) const
{
    return m_smartTaskNamesById.value( id );
}

QString SmartNameCache::makeCombined( const Task& task ) const {
    Q_ASSERT( task.isValid() || task.name().isEmpty() ); // an invalid task (id == 0) must not have a name
    if ( !task.isValid() )
        return QString();
    const Task parent = findTask( task.parent() );

    if ( parent.isValid() )
        return QObject::tr( "%1/%2", "parent task name/task name" ).arg( parent.name(), task.name() );
    else
        return task.name();
}

void SmartNameCache::regenerateSmartNames()
{
    m_smartTaskNamesById.clear();
    typedef QPair<TaskId, TaskId> TaskParentPair;

    QMap<QString, QVector<TaskParentPair> > byName;

    Q_FOREACH( const Task& task, m_tasks )
        byName[makeCombined(task)].append( qMakePair( task.id(), task.parent() ) );

    QSet<QString> cannotMakeUnique;

    while ( !byName.isEmpty() ) {
        QMap<QString, QVector<TaskParentPair> > newByName;
        for ( QMap<QString, QVector<TaskParentPair> >::ConstIterator it = byName.constBegin();
              it != byName.constEnd();
              ++it ) {
            const QString currentName = it.key();
            const QVector<TaskParentPair>& taskPairs = it.value();
            Q_ASSERT( !taskPairs.isEmpty() );
            if ( taskPairs.size() == 1 || cannotMakeUnique.contains( currentName ) ) {
                Q_FOREACH( const TaskParentPair& i, taskPairs )
                    m_smartTaskNamesById.insert( i.first, currentName );
            } else {
                Q_FOREACH( const TaskParentPair& taskPair, taskPairs ) {
                    const Task parent = findTask( taskPair.second );
                    if ( parent.isValid() ) {
                        const QString newName = parent.name() + QLatin1Char('/') + currentName;
                        newByName[newName].append( qMakePair( taskPair.first, parent.parent() ) );
                    } else {
                        const auto existing = newByName.constFind( currentName );
                        if ( existing != newByName.constEnd() )
                            cannotMakeUnique.insert( currentName );
                        newByName[currentName].append( taskPair );
                    }
                }
            }
        }
        byName = newByName;
    }
}
