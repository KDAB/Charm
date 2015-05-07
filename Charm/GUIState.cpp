/*
  GUIState.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2007-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>

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

#include "GUIState.h"

#include "Core/CharmConstants.h"
#include "Core/Task.h"

#include <QList>
#include <QSettings>

GUIState::GUIState()
    : m_selectedTask( 0 )
    , m_showExpired( false )
    , m_showCurrents( false )
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

bool GUIState::showExpired() const
{
    return m_showExpired;
}

bool GUIState::showCurrents() const
{
    return m_showCurrents;
}

void GUIState::setSelectedTask( TaskId task )
{
    m_selectedTask = task;
}

void GUIState::setExpandedTasks( const TaskIdList& tasks )
{
    m_expandedTasks = tasks;
}

void GUIState::setShowExpired( bool show )
{
    m_showExpired = show;
}

void GUIState::setShowCurrents( bool show )
{
    m_showCurrents = show;
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
    settings.setValue( MetaKey_MainWindowGUIStateShowExpiredTasks, showExpired() );
    settings.setValue( MetaKey_MainWindowGUIStateShowCurrentTasks, showCurrents() );
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
        setShowExpired( settings.value( MetaKey_MainWindowGUIStateShowExpiredTasks ).toBool() );
        setShowCurrents( settings.value( MetaKey_MainWindowGUIStateShowCurrentTasks ).toBool() );
    }
}
