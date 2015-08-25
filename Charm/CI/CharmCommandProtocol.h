/*
  CharmCommandProtocol.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2015-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Guillermo A. Amaral <gamaral@kdab.com>

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

#ifndef CHARM_CI_CHARMCOMMANDPROTOCOL_H
#define CHARM_CI_CHARMCOMMANDPROTOCOL_H

#define CHARM_CI_VERSION                    0x0001

#define CHARM_CI_COMMAND_DISCONNECT         "BYE"
#define CHARM_CI_COMMAND_RECENT             "RECENT"
#define CHARM_CI_COMMAND_START              "START"
#define CHARM_CI_COMMAND_STATUS             "STATUS"
#define CHARM_CI_COMMAND_STOP               "STOP"
#define CHARM_CI_COMMAND_TASK               "TASK"
#define CHARM_CI_EVENT_TASK_ACTIVATED       "TASK ACTIVATED"
#define CHARM_CI_EVENT_TASK_ADDED           "TASK ADDED"
#define CHARM_CI_EVENT_TASK_DEACTIVATED     "TASK DEACTIVATED"
#define CHARM_CI_EVENT_TASK_MODIFIED        "TASK MODIFIED"
#define CHARM_CI_EVENT_TASK_RESET           "TASK RESET"
#define CHARM_CI_HANDSHAKE_RECV             "READY"
#define CHARM_CI_HANDSHAKE_SEND             "HELLO CI"
#define CHARM_CI_SERVER_ACK                 "ACK"
#define CHARM_CI_SERVER_COMMENT             "*"
#define CHARM_CI_SERVER_NAK                 "NAK"

#endif // CHARM_CI_CHARMCOMMANDPROTOCOL_H

