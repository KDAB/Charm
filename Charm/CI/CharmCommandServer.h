#ifndef CHARM_CI_CHARMCOMMANDSERVER_H
#define CHARM_CI_CHARMCOMMANDSERVER_H

#include <QObject>

class QIODevice;

class CharmCommandServer : public QObject
{
    Q_OBJECT
public:
    explicit CharmCommandServer(QObject* parent = nullptr);
    ~CharmCommandServer();

    virtual bool listen() = 0;
    virtual void close() = 0;

protected:
    void spawnSession(QIODevice* device);
};

#endif // CHARM_CI_CHARMCOMMANDSERVER_H
