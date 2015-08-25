/*
  CharmLocalCommandServer.cpp

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

#include "CharmLocalCommandServer.h"

#include <QDir>
#include <QLocalServer>
#include <QLocalSocket>

#include "CharmCommandSession.h"

#include "CharmCMake.h"

#ifndef CHARM_CI_LOCALSERVER
#error Build system error: CHARM_CI_LOCALSERVER should be defined
#endif

CharmLocalCommandServer::CharmLocalCommandServer(QObject* parent)
    : CharmCommandServer(parent)
    , m_server(new QLocalServer(this))
{
}

CharmLocalCommandServer::~CharmLocalCommandServer()
{
}

bool CharmLocalCommandServer::listen()
{
    const QString name(QDir::tempPath() + '/' + "charm.sock");

#ifdef Q_OS_UNIX
    QFile::remove(name); // Try to clean up stale socket if possible
#endif

    if (!m_server->listen(name)) {
        qWarning("Failed to listen on %s", qPrintable(name));
        return false;
    }

    connect(m_server, SIGNAL(newConnection()), SLOT(onNewConnection()));

    return true;
}

void CharmLocalCommandServer::close()
{
    m_server->close();
}

void CharmLocalCommandServer::onNewConnection()
{
    while (m_server->hasPendingConnections()) {
        QLocalSocket* conn = m_server->nextPendingConnection();
        Q_ASSERT(conn);

        qDebug("New Local connection, creating command session...");
        spawnSession(conn);
    }
}

