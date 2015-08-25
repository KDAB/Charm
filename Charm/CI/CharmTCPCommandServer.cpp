/*
  CharmTCPCommandServer.cpp

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

#include "CharmTCPCommandServer.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QTimerEvent>
#include <QUdpSocket>

#include "CharmCommandSession.h"

#include "CharmCMake.h"

#ifndef CHARM_CI_TCPSERVER
#error Build system error: CHARM_CI_TCPSERVER should be defined
#endif

static const quint16 sCharmDefaultPort(5323);
static const int sCharmDiscoveryBroadcastRate(5000);

CharmTCPCommandServer::CharmTCPCommandServer(QObject* parent)
    : CharmCommandServer(parent)
    , m_address(QHostAddress::Any)
    , m_port(sCharmDefaultPort)
    , m_server(new QTcpServer(this))
    , m_discovery(new QUdpSocket(this))
    , m_discoveryTimer(0)
{
}

CharmTCPCommandServer::~CharmTCPCommandServer()
{
}

const QHostAddress & CharmTCPCommandServer::address() const
{
    return m_address;
}

void CharmTCPCommandServer::setAddress(const QHostAddress &address)
{
    m_address = address;
}

quint16 CharmTCPCommandServer::port() const
{
    return m_port;
}

void CharmTCPCommandServer::setPort(quint16 port)
{
    m_port = port;
}

bool CharmTCPCommandServer::listen()
{
    if (!m_server->listen(m_address, m_port)) {
        qWarning("Failed to bind to %s:%d", qPrintable(m_address.toString()), m_port);
        return false;
    }

    connect(m_server, SIGNAL(newConnection()), SLOT(onNewConnection()));

    m_discoveryTimer = startTimer(sCharmDiscoveryBroadcastRate);

    return true;
}

void CharmTCPCommandServer::close()
{
    if (m_discoveryTimer)
        killTimer(m_discoveryTimer), m_discoveryTimer = 0;

    m_server->close();
}

void CharmTCPCommandServer::timerEvent(QTimerEvent* event)
{
    if (event->timerId() != m_discoveryTimer)
        return;

    static const char sBroadcastIdentifier[] = "LUCKY"; // CHARMS

    m_discovery->writeDatagram(sBroadcastIdentifier, sizeof(sBroadcastIdentifier),
       QHostAddress::Broadcast, m_port);
}

void CharmTCPCommandServer::onNewConnection()
{
    while (m_server->hasPendingConnections()) {
        QTcpSocket* conn = m_server->nextPendingConnection();
        Q_ASSERT(conn);

        qDebug("New TCP connection, creating command session...");
        spawnSession(conn);
    }
}

