#include <QList>
#include <QtDebug>
#include <QSettings>

#include "Core/CharmConstants.h"

#include "Core/Task.h"
#include "GUIState.h"

GUIState::GUIState()
    : m_selectedTask( 0 )
{
}

const TaskIdList& GUIState::expandedTasks() const
{
    return m_expandedTasks;
}

TaskId GUIState::selectedTask() const
{
    return m_selectedTask;
}

void GUIState::setSelectedTask( TaskId task )
{
    m_selectedTask = task;
}

void GUIState::setExpandedTasks( const TaskIdList& tasks )
{
    m_expandedTasks = tasks;
}

void GUIState::saveTo( QSettings& settings )
{
    settings.setValue( MetaKey_MainWindowGUIStateSelectedTask, selectedTask() );
    // workaround for not getting QVariant serialization of TaskIdLists to work:
    QList<QVariant> variants;
    Q_FOREACH( TaskId v, expandedTasks() ) {
        variants << v;
    }
    settings.setValue( MetaKey_MainWindowGUIStateExpandedTasks, variants );
}

void GUIState::loadFrom( const QSettings& settings )
{
    if ( settings.contains( MetaKey_MainWindowGUIStateSelectedTask ) ) {
        setSelectedTask( settings.value( MetaKey_MainWindowGUIStateSelectedTask ).value<TaskId>() );;
    }
    if ( settings.contains( MetaKey_MainWindowGUIStateExpandedTasks ) ) {
        // workaround for not getting QVariant serialization of TaskIdLists to work:
        QList<QVariant> values( settings.value( MetaKey_MainWindowGUIStateExpandedTasks ).value<QList<QVariant> >() );
        TaskIdList ids;
        Q_FOREACH( QVariant variant, values ) {
            ids << variant.value<TaskId>();
        }
        setExpandedTasks( ids );
    }
}
