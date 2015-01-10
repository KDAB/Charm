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

