#ifndef CHARM_CI_CHARMCOMMANDSESSION_H
#define CHARM_CI_CHARMCOMMANDSESSION_H

#include <QObject>

#include "Core/CharmDataModelAdapterInterface.h"

class QIODevice;

class CharmCommandSession : public QObject,
                            public CharmDataModelAdapterInterface
{
    enum State
    {
        InvalidState   = 0,
        HandshakeState = 1,
        CommandState   = 2
    };

    Q_OBJECT
public:
    explicit CharmCommandSession(QObject* parent = nullptr);
    ~CharmCommandSession();

    QIODevice* device() const;
    void setDevice(QIODevice* device);

public: /* CharmDataModelAdapterInterface */
    void resetTasks();
    void taskAboutToBeAdded( TaskId, int ) {};
    void taskAdded( TaskId );
    void taskModified( TaskId );
    void taskParentChanged( TaskId, TaskId, TaskId ) {};
    void taskAboutToBeDeleted( TaskId ) {};
    void taskDeleted( TaskId ) {};

    void resetEvents() {};
    void eventAboutToBeAdded( EventId id ) {};
    void eventAdded( EventId id ) {};
    void eventModified( EventId id, Event discardedEvent ) {};
    void eventAboutToBeDeleted( EventId id ) {};
    void eventDeleted( EventId id ) {};

    void eventActivated( EventId id );
    void eventDeactivated( EventId id );

protected:
    void reset();

private slots:
    void onReadyRead();

private:
    void sendAck(const QString &comment);
    void sendNak(const QString &comment);
    void sendComment(const QString &comment);

private:
    void startHandshake();
    void startCommand();
    void handleHandshare(QByteArray payload);
    void handleCommand(QByteArray payload);

private:
    QIODevice* m_device;
    State m_state;
};

#endif // CHARM_CI_CHARMCOMMANDSESSION_H
