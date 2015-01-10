#ifndef CHARM_CI_CHARMTCPSERVER_H
#define CHARM_CI_CHARMTCPSERVER_H

#include "CharmCommandServer.h"

#include <QHostAddress>

class QTcpServer;
class QUdpSocket;

class CharmTCPCommandServer : public CharmCommandServer
{
    Q_OBJECT
public:
    explicit CharmTCPCommandServer(QObject* parent = nullptr);
    ~CharmTCPCommandServer();

    const QHostAddress & address() const;
    void setAddress(const QHostAddress &address);

    quint16 port() const;
    void setPort(quint16 port);

    bool listen() override;
    void close() override;

protected: /* reimpl */
    void timerEvent(QTimerEvent *event);

private slots:
    void onNewConnection();

private:
    QHostAddress m_address;
    quint16 m_port;

    QTcpServer* m_server;
    QUdpSocket* m_discovery;
    int m_discoveryTimer;
};

#endif // CHARM_CI_CHARMTCPSERVER_H
