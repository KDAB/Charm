#ifndef CONTROLLERTESTS_H
#define CONTROLLERTESTS_H

#include <QObject>

#include "Core/Configuration.h"
#include "Core/ControllerInterface.h"

class ControllerTests : public QObject
{
    Q_OBJECT

public:
    ControllerTests();

public slots: // not test cases
    void slotCurrentEvents( const EventList& );

    void slotDefinedTasks( const TaskList& );

    void slotTaskAdded( const Task& );
    void slotTaskUpdated( const Task& );
    void slotTaskDeleted( const Task& );

private slots:
    void initTestCase ();

    void initializeConnectBackendTest();

    void persistProvideMetaDataTest();

    void getDefinedTasksTest();

    void addModifyDeleteTaskTest();

    void toAndFromXmlTest();

    // this is now done by the model:
    // void startModifyEndEventTest();

    void disconnectFromBackendTest();

    void cleanupTestCase();

private:

    ControllerInterface* m_controller;
    Configuration& m_configuration;
    QString m_localPath;
    EventList m_currentEvents;
    bool m_eventListReceived;
    TaskList m_definedTasks;
    bool m_taskListReceived;
};

#endif
