/*
  ViewHelpers.h

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

#ifndef VIEWHELPERS_H
#define VIEWHELPERS_H

#include "Core/Event.h"
#include "Core/CharmConstants.h"

#include "ApplicationCore.h"
#include "ModelConnector.h"

#define MODEL ( ApplicationCore::instance().model() )
#define DATAMODEL ( MODEL.charmDataModel() )
#define VIEW ( ApplicationCore::instance().mainView() )
#define TRAY ( ApplicationCore::instance().trayIcon() )

namespace Charm {
    void connectControllerAndView( Controller*, CharmWindow* );
    EventIdList eventIdsSortedByStartTime( EventIdList );
    /** Return those ids in the input list that elements of the subtree
     * under the parent task, which includes the parent task. */
    EventIdList filteredBySubtree( EventIdList, TaskId parent, bool exclude=false );
    QString elidedTaskName( const QString& text, const QFont& font, int width );
    QString reportStylesheet( const QPalette& palette );
}

#endif
