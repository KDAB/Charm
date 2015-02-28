/*
  ViewInterface.h

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

#ifndef CHARM_VIEWINTERFACE_H
#define CHARM_VIEWINTERFACE_H

#include "Task.h"
#include "State.h"
#include "Event.h"

class CharmCommand;

class ViewInterface
{
public:
    virtual ~ViewInterface() {} // keep compiler happy

    // application:
    virtual void stateChanged( State previous ) = 0;
    virtual void visibilityChanged( bool ) = 0; // implement as signal and emit from show and hide events
    virtual void configurationChanged() = 0;
    virtual void saveConfiguration() = 0;

    virtual void emitCommand( CharmCommand* ) = 0;
    virtual void emitCommandRollback( CharmCommand* ) = 0;
    virtual void sendCommand( CharmCommand* ) = 0;
    virtual void sendCommandRollback( CharmCommand* ) = 0;
    virtual void commitCommand( CharmCommand* ) = 0;

    // restore the view
    virtual void restore() = 0;
    // quit the application
    virtual void quit() = 0;

};

#endif
