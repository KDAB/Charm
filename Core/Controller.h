#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>

#include "Task.h"
#include "Event.h"
#include "ControllerInterface.h"

class StorageInterface;

class Controller : public QObject,
                   public ControllerInterface
{
    Q_OBJECT

public:
    explicit Controller( QObject* parent = 0 );
    ~Controller();

    void stateChanged( State previous, State next );
    void persistMetaData( Configuration& );
    void provideMetaData( Configuration& );

    bool initializeBackEnd( const QString& name );
    bool connectToBackend();
    bool disconnectFromBackend();
    StorageInterface* storage();

    // FIXME add the add/modify/delete functions will not be slots anymore
    Event makeEvent( const Task& );
    Event cloneEvent( const Event& );
    bool modifyEvent( const Event& );
    bool deleteEvent( const Event& );

    bool addTask( const Task& parent );
    bool modifyTask( const Task& );
    bool deleteTask( const Task& );
    bool setAllTasks( const TaskList& );
    QDomDocument exportDatabasetoXml() const;
    QString importDatabaseFromXml( const QDomDocument& );

    void updateModelEventsAndTasks();

public slots:

    void executeCommand( CharmCommand* );
    void rollbackCommand ( CharmCommand* );

signals:
    void eventAdded( const Event& event );
    void eventModified( const Event& event );
    void eventDeleted( const Event& event );
    void allEvents( const EventList& );
    void definedTasks( const TaskList& );
    void taskAdded( const Task& );
    void taskUpdated( const Task& );
    void taskDeleted( const Task& );
    void readyToQuit();
    void currentBackendStatus( const QString& text );

    void commandCompleted( CharmCommand* );

private:
    void updateSubscriptionForTask( const Task& );

    template<class T> void loadConfigValue( const QString &key, T &configValue ) const;
    StorageInterface* m_storage;
};

#endif
