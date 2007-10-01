#include <QtDebug>

#include "Task.h"

Task::Task()
    : m_id( 0 )
    , m_parent ( 0 )
    , m_subscribed( false )
{
}

Task::Task( TaskId id, const QString& name, TaskId parent, bool subscribed )
    : m_id( id )
    , m_parent( parent )
    , m_name( name )
    , m_subscribed( subscribed )
{
}

bool Task::isValid() const
{
    return id() != 0;
}

bool Task::operator == ( const Task& other ) const
{
    return
        other.id() == id()
        && other.parent() == parent()
        && other.name() == name()
        && other.subscribed() == subscribed();
}

TaskId Task::id() const
{
    return m_id;
}

void Task::setId( TaskId id )
{
    m_id = id;
}

QString Task::name() const
{
    return m_name;
}

void Task::setName( const QString& name )
{
    m_name = name;
}

int Task::parent() const
{
    return m_parent;
}

void Task::setParent( int parent )
{
    m_parent = parent;
}

bool Task::subscribed() const
{
    return m_subscribed;
}

void Task::setSubscribed( bool value )
{
    m_subscribed = value;
}

void Task::dump() const
{
    qDebug() << "[Task " << this << "] task id:" << id()
             << "- name:" << name()
             << " - parent:" << parent()
             << " - subscribed:" << subscribed();
}

void dumpTaskList( const TaskList& tasks )
{
    qDebug() << "dumpTaskList: task list of" << tasks.size() << "elements";
    for ( int i = 0; i < tasks.size(); ++i )
    {
        tasks[i].dump();
    }
}

