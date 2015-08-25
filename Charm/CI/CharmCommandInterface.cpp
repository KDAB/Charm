/*
  CharmCommandInterface.cpp

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

#include "CharmCommandInterface.h"

#include "Core/CharmConstants.h"

#include "CharmCommandServer.h"

#include "CharmCMake.h"

#ifndef CHARM_CI_SUPPORT
#error Build system error: CHARM_CI_SUPPORT should be defined
#endif

#ifdef CHARM_CI_TCPSERVER
#  include "CharmTCPCommandServer.h"
#endif
#ifdef CHARM_CI_LOCALSERVER
#  include "CharmLocalCommandServer.h"
#endif

CharmCommandInterface::CharmCommandInterface(QObject* parent)
    : QObject(parent)
{
}

CharmCommandInterface::~CharmCommandInterface()
{
    stop();
}

bool CharmCommandInterface::isStarted() const
{
    return (!m_servers.isEmpty());
}

void CharmCommandInterface::start()
{
    if (!CONFIGURATION.enableCommandInterface ||
        isStarted())
        return;

    // Create command line interface servers
    //
#ifdef CHARM_CI_TCPSERVER
    m_servers.append(new CharmTCPCommandServer);
#endif
#ifdef CHARM_CI_LOCALSERVER
    m_servers.append(new CharmLocalCommandServer);
#endif

    if (m_servers.isEmpty()) {
        qDebug("No command interface servers available!");
        return;
    }

    qDebug("Starting command interface servers...");
    foreach (CharmCommandServer *server, m_servers)
        server->listen();
}

void CharmCommandInterface::stop()
{
    if (!isStarted())
        return;

    qDebug("Stopping command interface servers...");
    foreach (CharmCommandServer *server, m_servers) {
        server->close();
        server->deleteLater();
    }

    m_servers.clear();
}

void CharmCommandInterface::configurationChanged()
{
    if (CONFIGURATION.enableCommandInterface && !isStarted())
        start();
    else if (!CONFIGURATION.enableCommandInterface && isStarted())
        stop();
}
