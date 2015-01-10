#ifndef CHARM_CI_CHARMLOCALSERVER_H
#define CHARM_CI_CHARMLOCALSERVER_H

#include "CharmCommandServer.h"

class QLocalServer;

class CharmLocalCommandServer : public CharmCommandServer
{
    Q_OBJECT
public:
    explicit CharmLocalCommandServer(QObject* parent = nullptr);
    ~CharmLocalCommandServer();

    bool listen() override;
    void close() override;

private slots:
    void onNewConnection();

private:
    QLocalServer* m_server;
};

#endif // CHARM_CI_CHARMLOCALSERVER_H
