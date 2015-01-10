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
