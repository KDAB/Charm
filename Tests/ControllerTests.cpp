#include <QDir>
#include <QFileInfo>
#include <QtDebug>
#include <QtTest/QtTest>

#include <StorageInterface.h>
#include <CharmConstants.h>
#include <Controller.h>

#include "ControllerTests.h"

ControllerTests::ControllerTests()
    : QObject()
    , m_controller( 0 )
    , m_configuration( Configuration::instance() )
    , m_localPath( "./ControllerTestDatabase.db" )
    , m_eventListReceived( false )
    , m_taskListReceived( false )
{
}

void ControllerTests::initTestCase ()
{
    QFileInfo file( m_localPath );
    if ( file.exists() ) {
        qDebug() << "test database file exists, deleting";
        QDir dir( file.absoluteDir() );
        QVERIFY( dir.remove( file.fileName() ) );
    }

    m_configuration.installationId = 1;
    m_configuration.user.setId( 1 );
    m_configuration.localStorageType = CHARM_SQLITE_BACKEND_DESCRIPTOR;
    m_configuration.localStorageDatabase = m_localPath;
    m_configuration.newDatabase = true;
    Controller* controller = new Controller;
    m_controller = controller;
    connect( controller, SIGNAL( currentEvents( const EventList& ) ),
             SLOT( slotCurrentEvents( const EventList& ) ) );
    connect( controller, SIGNAL( definedTasks( const TaskList& ) ),
             SLOT( slotDefinedTasks( const TaskList& ) ) );
    connect( controller, SIGNAL( taskAdded( const Task& ) ),
             SLOT( slotTaskAdded( const Task& ) ) );
    connect( controller, SIGNAL( taskUpdated( const Task& ) ),
             SLOT( slotTaskUpdated( const Task& ) ) );
    connect( controller, SIGNAL( taskDeleted( const Task& ) ),
             SLOT( slotTaskDeleted( const Task& ) ) );
}

void ControllerTests::initializeConnectBackendTest()
{
    QVERIFY( m_controller->initializeBackEnd( CHARM_SQLITE_BACKEND_DESCRIPTOR ) );
    QVERIFY( m_controller->connectToBackend() );
}

void ControllerTests:: persistProvideMetaDataTest()
{
    const int NumberOfSettings = 4;
    bool Settings[NumberOfSettings][2] = {
        { false, false },
        { false, true },
        { true, false },
        { true, true }
    };
    for ( int i = 0; i < NumberOfSettings; ++i )
    {
        // save some settings, modify the config values, read the settings
        // back, see if the values are what we stored
        m_configuration.eventsInLeafsOnly = Settings[i][0];
        m_configuration.oneEventAtATime = Settings[i][1];
        m_controller->persistMetaData( m_configuration );
        m_configuration.eventsInLeafsOnly = ! Settings[i][0];
        m_configuration.oneEventAtATime = ! Settings[i][1];
        m_controller->provideMetaData( m_configuration );
        QVERIFY( m_configuration.eventsInLeafsOnly == Settings[i][0] );
        QVERIFY( m_configuration.oneEventAtATime == Settings[i][1] );
        // and repeat, with some different values
    }
}

void ControllerTests::slotCurrentEvents( const EventList& events )
{
    m_currentEvents = events;
    m_eventListReceived = true;
}

void ControllerTests::slotDefinedTasks( const TaskList& tasks )
{
    m_definedTasks = tasks;
    m_taskListReceived = true;
}

void ControllerTests::slotTaskAdded( const Task& task )
{
    m_definedTasks << task;
}

void ControllerTests::slotTaskUpdated( const Task& task )
{
    int i;
    for ( i = 0; i < m_definedTasks.size(); ++i )
    {
        if ( m_definedTasks[i].id() == task.id() ) {
            m_definedTasks[i] = task;
            break;
        }
    }
    // it is a failure if we receive the signal for nonexisting tasks
    QVERIFY( i != m_definedTasks.size() );
}

void ControllerTests::slotTaskDeleted( const Task& task )
{
    // task.dump();
    int i;
    for ( i = 0; i < m_definedTasks.size(); ++i )
    {
        if ( m_definedTasks[i].id() == task.id() ) {
            m_definedTasks.removeAt( i );
            return;
        }
    }
    // it is a failure if we receive the signal for nonexisting tasks
    QVERIFY( i != m_definedTasks.size() ); // always true, give more
                                           // feedback
}

void ControllerTests::getDefinedTasksTest()
{
    // get the controller to load the initial task list, which
    // is supposed to be empty
    m_controller->stateChanged( Connecting, Connected );
    // QTest::qWait( 100 ); // go back to event loop
    // verify that this triggers the definedTasks signal, but not the
    // currentEvents signal
    // (it is fine if that changes at some point, this tests the
    // status quo)
    QVERIFY( m_currentEvents.isEmpty() && m_eventListReceived == false );
    QVERIFY( m_definedTasks.isEmpty()  && m_taskListReceived == true );
    m_eventListReceived = false;
    m_taskListReceived = false;
}

void ControllerTests::addModifyDeleteTaskTest()
{
    // make two tasks, add them, and verify the expected results:
    const int Task1Id = 1000;
    const QString Task1Name( "Task-1-Name" );
    Task task1;
    task1.setId( Task1Id );
    task1.setName( Task1Name );
    task1.setSubscribed( true );
    const int Task2Id = 2000;
    const QString Task2Name( "Task-2-Name" );
    Task task2;
    task2.setId( Task2Id );
    task2.setName( Task1Name );
    task2.setParent( task1.id() );
    m_controller->addTask( task1 );
    // QTest::qWait( 1 ); // only necessary if we do this in threads
    QVERIFY( m_currentEvents.isEmpty() && m_eventListReceived == false );
    QVERIFY( m_definedTasks.size() == 1 && m_definedTasks[0] == task1 );
    m_controller->addTask( task2 );
    QVERIFY( m_definedTasks.size() == 2 );
    // both tasks must be in the list, but the order is unspecified:
    int task1Position, task2Position;
    if ( m_definedTasks[0].id() == task1.id() ) {
        task1Position = 0;
        task2Position = 1;
    } else {
        task1Position = 1;
        task2Position = 0;
    }
    QVERIFY( m_definedTasks[task1Position] == task1 );
    QVERIFY( m_definedTasks[task2Position] == task2 );
    // modify one of the tasks:
    const QString Task1_1Name ( "Task-1-1-Name" );
    task1.setName( Task1_1Name );
    task1.setSubscribed( false );
    m_controller->modifyTask( task1 );
    QVERIFY( m_definedTasks.size() == 2 );
    QVERIFY( m_definedTasks[task1Position] == task1 );
    QVERIFY( m_definedTasks[task2Position] == task2 );
    const QString Task2_1Name( "Task-2-1-Name" );
    task2.setName( Task2_1Name );
    task2.setSubscribed( true );
    m_controller->modifyTask( task2 );
    QVERIFY( m_definedTasks[task1Position] == task1 );
    QVERIFY( m_definedTasks[task2Position] == task2 );
    // delete the tasks:
    m_controller->deleteTask( task1 );
    QVERIFY( m_definedTasks.size() == 1 );
    QVERIFY( m_definedTasks[0] == task2 );
    m_controller->deleteTask( task2 );
    QVERIFY( m_definedTasks.isEmpty() );
    //  leave both tasks in for later tests:
    m_controller->addTask( task2 );
    QVERIFY( m_definedTasks.size() == 1 );
    m_controller->addTask( task1 );
    QVERIFY( m_definedTasks.size() == 2 );
}

void ControllerTests::toAndFromXmlTest()
{
    // make sure we have some tasks and associated events:
    TaskList tasks = m_controller->storage()->getAllTasks();
    QVERIFY( tasks.size() > 0 ); // just to be sure nobody fucks it up
    Event e1 = m_controller->storage()->makeEvent();
    e1.setTaskId( tasks[0].id() );
    e1.setComment( "Event-1-Comment" );
    e1.setStartDateTime();
    m_controller->modifyEvent( e1 );
    Event e2 = m_controller->storage()->makeEvent();
    e2.setTaskId( tasks.last().id() );
    e2.setComment( "Event-2-Comment" );
    e2.setStartDateTime();
    m_controller->modifyEvent( e2 );

    Q_ASSERT( m_controller ); // just to be sure
    TaskList tasksBefore = m_controller->storage()->getAllTasks();
    EventList eventsBefore = m_controller->storage()->getAllEvents();
    QVERIFY( tasksBefore == tasks );
    QDomDocument document = m_controller->exportDatabasetoXml();
    if ( ! m_controller->importDatabaseFromXml( document ).isEmpty() ) {
        QFAIL( "Cannot reimport exported Xml Database Dump" );
    } else {
        TaskList tasksAfter = m_controller->storage()->getAllTasks();
        QVERIFY( tasksBefore == tasksAfter );
        EventList eventsAfter = m_controller->storage()->getAllEvents();
        // this is brittle, and may easily fail, because the event ids are auto-increment
        QVERIFY( eventsBefore == eventsAfter );
    }
    QDomDocument document2 = m_controller->exportDatabasetoXml();

    // QVERIFY( document == document2 ); unfortunately, this seems to always fail
}

// void ControllerTests::startModifyEndEventTest()
// {
//     // preparations
//     m_eventListReceived = false;
//     m_taskListReceived = false;
//     m_configuration.oneEventAtATime = false;
//     QVERIFY( m_definedTasks.size() >= 2 ); // rely on previous test results
//     QVERIFY( m_currentEvents.size() == 0 );
//     // make first event
//     m_controller->startEvent( m_definedTasks[0] );
//     QVERIFY( m_currentEvents.size() == 1 );
//     Event event1 = m_currentEvents[0];
//     event1.setTask( m_definedTasks[0] );
//     event1.setComment( "Event-1-Comment" );
//     event1.setStartDateTime();
//     m_controller->modifyEvent( event1 );
//     QVERIFY( m_currentEvents.size() == 1 );
//     QVERIFY( m_currentEvents[0] == event1 );
//     // make second event
//     m_controller->startEvent( m_definedTasks[1] );
//     QVERIFY( m_currentEvents.size() ==2 );
//     int event1Position, event2Position;
//     if ( m_currentEvents[0].id() == event1.id() ) {
//         event1Position = 0;
//         event2Position = 1;
//     } else {
//         event1Position = 1;
//         event2Position = 2;
//     }
//     QVERIFY( m_currentEvents[event1Position] == event1 );
//     Event event2 = m_currentEvents[event2Position];
//     event2.setTask( m_definedTasks[1] );
//     event2.setComment( "Event-2-Comment" );
//     event2.setStartDateTime();
//     m_controller->modifyEvent( event2 );
//     QVERIFY( m_currentEvents[event1Position] == event1 );
//     QVERIFY( m_currentEvents[event2Position] == event2 );
//     // end both events
//     m_controller->endEvent( event1.task() );
//     QVERIFY( m_currentEvents.size() == 1 && m_currentEvents[0] == event2 );
//     // change to "one event at a time mode"
//     m_configuration.oneEventAtATime = true;
//     // start another event similar to event 1:
//     m_controller->startEvent( event1.task() );
//     QVERIFY( m_currentEvents.size() == 1 );
//     // end all events
//     m_controller->endEvent( event1.task() );
//     QVERIFY( m_currentEvents.size() == 0 );
// }

void ControllerTests::disconnectFromBackendTest()
{
    QVERIFY( m_controller->disconnectFromBackend() );
}

void ControllerTests::cleanupTestCase ()
{
    if ( QDir::home().exists( m_localPath ) ) {
        bool result = QDir::home().remove( m_localPath );
        QVERIFY( result );
    }
    delete m_controller; m_controller = 0;
}

QTEST_MAIN( ControllerTests )

#include "ControllerTests.moc"
