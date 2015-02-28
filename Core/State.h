/*
  State.h

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

#ifndef CHARM_STATE_H
#define CHARM_STATE_H

#include <QMetaType>

enum State {
    Constructed,
    StartingUp,
    Configuring,
    Connecting,
    Connected,
    Disconnecting,
    ShuttingDown,
    Dead,
    NumberOfCharmApplicationStates
};

Q_DECLARE_METATYPE( State )

extern const char* StateNames[NumberOfCharmApplicationStates];

#endif
