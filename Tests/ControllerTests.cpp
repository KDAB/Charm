/*
  ControllerTests.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2007-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>
  Author: Frank Osterfeld <frank.osterfeld@kdab.com>
  Author: Mike McQuaid <mike.mcquaid@kdab.com>
  Author: Guillermo A. Amaral <gamaral@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ControllerTests.h"

#include "Core/StorageInterface.h"
#include "Core/CharmConstants.h"
#include "Core/Controller.h"

#include <QDir>
#include <QFileInfo>
#include <QtDebug>
#include <QtTest/QtTest>

ControllerTests::ControllerTests()
    : QObject()
    , m_controller( nullptr )
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
    auto controller = new Controller;
    m_controller = controller;
//    connect( controller, SIGNAL(currentEvents(EventList)),
//             SLOT(slotCurrentEvents(EventList)) );
    connect( controller, SIGNAL(definedTasks(TaskList)),
             SLOT(slotDefinedTasks(TaskList)) );
    connect( controller, SIGNAL(taskAdded(Task)),
             SLOT(slotTaskAdded(Task)) );
    connect( controller, SIGNAL(taskUpdated(Task)),
             SLOT(slotTaskUpdated(Task)) );
    connect( controller, SIGNAL(taskDeleted(Task)),
             SLOT(slotTaskDeleted(Task)) );
}

void ControllerTests::initializeConnectBackendTest()
{
    QVERIFY( m_controller->initializeBackEnd( CHARM_SQLITE_BACKEND_DESCRIPTOR ) );
    QVERIFY( m_controller->connectToBackend() );
}

void ControllerTests:: persistProvideMetaDataTest()
{   // stick with user id 0, it is not persisted in the DB, and 0 is the default
    Configuration configs[] = {
        Configuration( true, true, User( "bla", 0 ), Configuration::TaskPrefilter_ShowAll, Configuration::TimeTrackerFont_Small,
                       Configuration::Minutes, true, Qt::ToolButtonIconOnly, true, true, true, false ),
        Configuration( true, false, User( "blub", 0 ), Configuration::TaskPrefilter_CurrentOnly, Configuration::TimeTrackerFont_Regular,
                       Configuration::Minutes, false, Qt::ToolButtonTextOnly, false, false, false, false ),
        Configuration( false, true, User(), Configuration::TaskPrefilter_SubscribedAndCurrentOnly, Configuration::TimeTrackerFont_Large,
                       Configuration::Minutes, true, Qt::ToolButtonTextBesideIcon, true, true, true, false ),
    };
    const int NumberOfConfigurations = sizeof configs / sizeof configs[0];

    for ( int i = 0; i < NumberOfConfigurations; ++i )
    {
        m_controller->persistMetaData( configs[i] );
        m_configuration = Configuration();
        m_controller->provideMetaData( m_configuration );
        m_configuration.dump();
        configs[i].dump();
        QVERIFY( m_configuration == configs[i] );
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
    task1.setValidFrom( QDateTime::currentDateTime() );
    const int Task2Id = 2000;
    const QString Task2Name( "Task-2-Name" );
    Task task2;
    task2.setId( Task2Id );
    task2.setName( Task1Name );
    task2.setParent( task1.id() );
    task2.setValidUntil( QDateTime::currentDateTime() );
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
        if( tasksBefore != tasksAfter ) {
                qDebug() << "XML Document created during failed test:" << endl
                                << document.toString();
                        Q_FOREACH( Task task, tasksBefore ) {
                                task.dump();
                        }
                        Q_FOREACH( Task task, tasksAfter ) {
                                task.dump();
                        }
        }
        QVERIFY( tasksBefore == tasksAfter );
        EventList eventsAfter = m_controller->storage()->getAllEvents();
    }
    QDomDocument document2 = m_controller->exportDatabasetoXml();
}

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
    delete m_controller; m_controller = nullptr;
}

QTEST_MAIN( ControllerTests )

#include "moc_ControllerTests.cpp"
