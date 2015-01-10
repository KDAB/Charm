#ifndef CHARM_CI_CHARMCOMMANDINTERFACE_H
#define CHARM_CI_CHARMCOMMANDINTERFACE_H

#include <QObject>

class CharmCommandServer;

class CharmCommandInterface : public QObject
{
    Q_OBJECT
public:
    explicit CharmCommandInterface(QObject* parent = nullptr);
    ~CharmCommandInterface();

    bool isStarted() const;
    void start();
    void stop();

public slots:
    void configurationChanged();

private:
    QList<CharmCommandServer *> m_servers;
};

#endif // CHARM_CI_CHARMCOMMANDINTERFACE_H
