#include <QtDebug>
#include <QtTest/QtTest>

#include "Core/Task.h"

#include "TaskStructureTests.h"

TaskStructureTests::TaskStructureTests()
    : QObject()
{
}

void TaskStructureTests::checkForUniqueTaskIdsTest_data()
{
    QTest::addColumn<TaskList>( "tasks" );
    QTest::addColumn<bool>( "unique" );

    // FIXME this will be read from resources:
    // set up test candidates:
    TaskList tasks;
    Task task1;
    task1.setName( "A task" );
    task1.setId( 1 );
    task1.setParent( 0 );
    task1.setSubscribed( true );
    task1.setValidFrom( QDateTime::currentDateTime() );
    Task task2;
    task2.setName( "Another task" );
    task2.setId( 2 );
    task2.setParent( 1 );
    task2.setSubscribed( false );
    task2.setValidUntil( QDateTime::currentDateTime() );

    tasks << task1 << task2;
    QTest::newRow( "Empty TaskList" ) << TaskList() << true;
    QTest::newRow( "Simple List" ) << tasks << true;

    // now add a cycle to the graph, and make it fail:
    Task task3;
    task3.setName( "3" );
    task3.setId( 3 );
    task3.setParent( 5 );
    Task task4;
    task4.setName( "4" );
    task4.setId( 4 );
    task4.setParent( 3 );
    Task task5;
    task5.setName( "5" );
    task5.setId( 5 );
    task5.setParent( 4 );
    TaskList cyclicTaskList;
    cyclicTaskList << task3 << task4 << task5;
    QTest::newRow( "Cyclic TaskList" ) << cyclicTaskList << false;
}

void TaskStructureTests::checkForUniqueTaskIdsTest()
{
    QFETCH( TaskList, tasks );
    QFETCH( bool,  unique );

    QCOMPARE( Task::checkForUniqueTaskIds( tasks ), unique );
}

void TaskStructureTests::checkForTreenessTest_data()
{
    QTest::addColumn<TaskList>( "tasks" );
    QTest::addColumn<bool>( "directed" );

    // FIXME this will be read from resources:
    // set up test candidates:
    TaskList tasks;
    Task task1;
    task1.setName( "A task" );
    task1.setId( 1 );
    task1.setParent( 0 );
    task1.setSubscribed( true );
    task1.setValidFrom( QDateTime::currentDateTime() );
    Task task2;
    task2.setName( "Another task" );
    task2.setId( 2 );
    task2.setParent( 1 );
    task2.setSubscribed( false );
    task2.setValidUntil( QDateTime::currentDateTime() );

    tasks << task1 << task2;
    QTest::newRow( "Empty TaskList" ) << TaskList() << true;
    QTest::newRow( "Simple List" ) << tasks << true;

    // now add a cycle to the graph, and make it fail:
    Task task3;
    task3.setName( "3" );
    task3.setId( 3 );
    task3.setParent( 5 );
    Task task4;
    task4.setName( "4" );
    task4.setId( 4 );
    task4.setParent( 3 );
    Task task5;
    task5.setName( "5" );
    task5.setId( 5 );
    task5.setParent( 4 );
    TaskList cyclicTaskList;
    cyclicTaskList << task3 << task4 << task5;
    QTest::newRow( "Cyclic TaskList" ) << cyclicTaskList << false;
    Q_ASSERT( false && "the last one should fail" );
}

void TaskStructureTests::checkForTreenessTest()
{
    QFETCH( TaskList, tasks );
    QFETCH( bool, directed );

    QCOMPARE( Task::checkForTreeness( tasks ), directed );
}

void TaskStructureTests::mergeTaskListsTest()
{
    qDebug() << "NI";
}

QTEST_MAIN( TaskStructureTests )

#include "TaskStructureTests.moc"


