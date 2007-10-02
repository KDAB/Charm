#include <QDir>
#include <QFileInfo>
#include <QtDebug>
#include <QtTest/QtTest>

#include "CharmConstants.h"
#include "Controller.h"
#include "TaskTreeItem.h"
#include "Configuration.h"
#include "CharmDataModel.h"
#include "StorageInterface.h"
#include "BackendIntegrationTests.h"

const int UserId = 1;
const int InstallationId = 1;

BackendIntegrationTests::BackendIntegrationTests()
    : QObject()
    , m_configuration( Configuration::instance() )
    , m_localPath( "./BackendIntegrationTestDatabase.db" )
{
}

void BackendIntegrationTests::initTestCase()
{
    QFileInfo file( m_localPath );
    if ( file.exists() ) {
        qDebug() << "test database file exists, deleting";
        QDir dir( file.absoluteDir() );
        QVERIFY( dir.remove( file.fileName() ) );
    }

    // well, here it gets a bit more challenging - this is not for
    // sissies:
    // - make a controller
    // - make it create a local storage backend
    // - make a data model and connect it to the controller
    // - stimulate the controller and see if the right content ends up
    //   in the database and the model
    // -----
    // ... make the controller:
    m_configuration.installationId = InstallationId;
    m_configuration.user.setId( UserId );
    m_configuration.localStorageType = CHARM_SQLITE_BACKEND_DESCRIPTOR;
    m_configuration.localStorageDatabase = m_localPath;
    m_configuration.newDatabase = true;
    m_controller = new Controller;
    // ... initialize the backend:
    QVERIFY( m_controller->initializeBackEnd( CHARM_SQLITE_BACKEND_DESCRIPTOR ) );
    QVERIFY( m_controller->connectToBackend() );
    // ... make the data model:
    m_model = new CharmDataModel;
    // ... connect model and controller:
    connectControllerAndModel( m_controller, m_model );
    QVERIFY( m_controller->storage() != 0 );
}

void BackendIntegrationTests::initialValuesTest()
{
    // storage:
    QVERIFY( m_controller->storage()->getAllTasks().isEmpty() );
    QVERIFY( m_controller->storage()->getAllEvents().isEmpty() );
    QVERIFY( m_controller->storage()->getUser( UserId ).isValid() );
    QVERIFY( m_controller->storage()->getInstallation( InstallationId ).isValid() );
    // model:
    QVERIFY( m_model->taskTreeItem( 0 ).childCount() == 0 );
}

void BackendIntegrationTests::simpleCreateModifyDeleteTaskTest()
{
    Task task1( 1000, "Task 1" );
    Task task1b( task1 );
    task1b.setName( "Task 1, modified" );
    // add:
    m_controller->addTask( task1 );
    QVERIFY( m_controller->storage()->getAllTasks().size() == 1 );
    QVERIFY( m_controller->storage()->getAllTasks().first() == task1 );
    QVERIFY( m_model->taskTreeItem( task1.id() ).task() == task1 );
    // modify:
    m_controller->modifyTask( task1b );
    QVERIFY( m_controller->storage()->getAllTasks().size() == 1 );
    QVERIFY( m_controller->storage()->getAllTasks().first() == task1b );
    QVERIFY( m_model->taskTreeItem( task1.id() ).task() == task1b );
    // delete:
    m_controller->deleteTask( task1 );
    QVERIFY( m_controller->storage()->getAllTasks().size() == 0 );
    QVERIFY( m_model->taskTreeItem( 0 ).childCount() == 0 );
}

void BackendIntegrationTests::biggerCreateModifyDeleteTaskTest()
{
    const TaskList& tasks = referenceTasks();
    // make sure everything is cleaned up:
    QVERIFY( m_controller->storage()->getAllTasks().isEmpty() );
    QVERIFY( m_model->taskTreeItem( 0 ).childCount() == 0 );

    // add one task after the other, and compare the lists in storage
    // and in the model:
    TaskList currentTasks;
    for ( int i = 0; i < tasks.size(); ++ i )
    {
        currentTasks.append( tasks[i] );
        m_controller->addTask( tasks[i] );
        QVERIFY( contentsEqual( m_controller->storage()->getAllTasks(), currentTasks ) );
        QVERIFY( contentsEqual( m_model->getAllTasks(), currentTasks ) );
    }
    // modify the tasks:
    for ( int i = 0; i < currentTasks.size(); ++i )
    {
        currentTasks[i].setName( currentTasks[i].name() + " - modified" );
        m_controller->modifyTask( currentTasks[i] );
        QVERIFY( contentsEqual( m_controller->storage()->getAllTasks(), currentTasks ) );
        QVERIFY( contentsEqual( m_model->getAllTasks(), currentTasks ) );
    }
    // delete the tasks (in reverse, because they depend on each
    // other):
    for ( int i = currentTasks.size(); i > 0; --i )
    {
        m_controller->deleteTask( currentTasks[i-1] );
        currentTasks.removeAt( i-1 );
        QVERIFY( contentsEqual( m_controller->storage()->getAllTasks(), currentTasks ) );
        QVERIFY( contentsEqual( m_model->getAllTasks(), currentTasks ) );
    }
    // all gone?
    QVERIFY( m_controller->storage()->getAllTasks().isEmpty() );
    QVERIFY( m_model->taskTreeItem( 0 ).childCount() == 0 );
}

void BackendIntegrationTests::cleanupTestCase ()
{
    QVERIFY( m_controller->disconnectFromBackend() );
    delete m_model; m_model = 0;
    delete m_controller; m_controller = 0;
    if ( QDir::home().exists( m_localPath ) ) {
        bool result = QDir::home().remove( m_localPath );
        QVERIFY( result );
    }
}

const TaskList& BackendIntegrationTests::referenceTasks()
{
    static TaskList Tasks;
    if ( Tasks.isEmpty() ) {
        Task task1( 1000, "Task 1" );
        Task task1_1( 1001, "Task 1-1", task1.id() );
        Task task1_2( 1002, "Task 1-2", task1.id() );
        Task task1_3( 1003, "Task 1-3", task1.id() );
        Task task2( 2000, "Task 2" );
        Task task2_1( 2100, "Task 2-1", task2.id() );
        Task task2_1_1( 2110, "Task 2-1-1", task2_1.id() );
        Task task2_1_2( 2120, "Task 2-1-2", task2_1.id() );
        Task task2_2( 2200, "Task 2-2", task2.id() );
        Task task2_2_1( 2210, "Task 2-2-1", task2_2.id() );
        Task task2_2_2( 2220, "Task 2-2-2", task2_2.id() );
        Tasks << task1 << task1_1 << task1_2 << task1_3
              << task2 << task2_1 << task2_1_1 << task2_1_2
              << task2_2 << task2_2_1 << task2_2_2;
    }
    return Tasks;
}

bool BackendIntegrationTests::contentsEqual( const TaskList& listref1,
                                             const TaskList& listref2 )
{
    TaskList list1( listref1 );
    TaskList list2( listref2 );

    for ( int i = 0; i < list1.size(); ++i )
    {
        for ( int j = 0; j < list2.size(); ++j )
        {
            if ( list2[j] == list1[i] ) {
                list2.removeAt( j );
            }
        }
    }

    return list2.isEmpty();
}

QTEST_MAIN( BackendIntegrationTests )

#include "BackendIntegrationTests.moc"

