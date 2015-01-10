#include "CharmCommandServer.h"

#include "CharmCommandSession.h"

#include "CharmCMake.h"

#ifndef CHARM_CI_SUPPORT
#error Build system error: CHARM_CI_SUPPORT should be defined
#endif

CharmCommandServer::CharmCommandServer(QObject* parent)
    : QObject(parent)
{
}

CharmCommandServer::~CharmCommandServer()
{
}

void CharmCommandServer::spawnSession(QIODevice* device)
{
    CharmCommandSession *session = new CharmCommandSession(this);
    session->setDevice(device);
    connect(device, SIGNAL(disconnected()), session, SLOT(deleteLater()));
}

