/*
  GUIState.h

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

#ifndef GUISTATE_H
#define GUISTATE_H

#include "Core/Task.h"

class QSettings;

// I am unsure if this is a good idea (making a class for this at
// all), depends on how it turns out in the future. If there are no
// more options than that, it can be merged back into View.
class GUIState
{
public:
    GUIState();

    const TaskIdList& expandedTasks() const;
    TaskId selectedTask() const;
    bool showExpired() const;
    bool showCurrents() const;

    void setSelectedTask( TaskId );
    void setExpandedTasks( const TaskIdList& );
    void setShowExpired( bool show );
    void setShowCurrents( bool show );

    void saveTo( QSettings& settings );
    void loadFrom( const QSettings& settings );

private:
    TaskIdList m_expandedTasks;
    TaskId m_selectedTask;
    bool m_showExpired; // show also expired tasks
    bool m_showCurrents; // show only selected tasks
};

#endif
