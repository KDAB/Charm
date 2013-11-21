#include <QtDebug>
#include <QtTest/QtTest>
#include <QDir>
#include <QFileInfo>

#include "Core/Task.h"
#include "Core/TaskListMerger.h"
#include "Core/CharmExceptions.h"
#include "Core/CharmConstants.h"

#include "TestHelpers.h"
#include "TaskStructureTests.h"

TaskStructureTests::TaskStructureTests()
    : QObject()
{
}

void TaskStructureTests::checkForUniqueTaskIdsTest_data()
{
    QTest::addColumn<TaskList>( "tasks" );
    QTest::addColumn<bool>( "unique" );

    Q_FOREACH( QDomElement testcase,
               TestHelpers::retrieveTestCases( ":/checkForUniqueTaskIdsTest/Data", "checkForUniqueTaskIdsTest" ) ) {
        QString name = testcase.attribute( "name" );
        bool expectedResult = TestHelpers::attribute( "expectedResult", testcase );
        QDomElement element = testcase.firstChildElement( Task::taskListTagName() );
        QVERIFY( !element.isNull() );
        TaskList tasks = Task::readTasksElement( element, CHARM_DATABASE_VERSION );
        QTest::newRow( name.toLocal8Bit() ) << tasks << expectedResult;
        QVERIFY( element.nextSiblingElement( Task::taskListTagName() ).isNull() );
        qDebug() << "Added test case" << name;
    }
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

    Q_FOREACH( const QDomElement& testcase,
               TestHelpers::retrieveTestCases( ":/checkForTreenessTest/Data", "checkForTreenessTest" ) ) {
        QString name = testcase.attribute( "name" );
        bool expectedResult = TestHelpers::attribute( "expectedResult", testcase );
        QDomElement element = testcase.firstChildElement( Task::taskListTagName() );
        QVERIFY( !element.isNull() );
        TaskList tasks = Task::readTasksElement( element, CHARM_DATABASE_VERSION );
        QTest::newRow( name.toLocal8Bit() ) << tasks << expectedResult;
        QVERIFY( element.nextSiblingElement( Task::taskListTagName() ).isNull() );
        qDebug() << "Added test case" << name;
    }
}

void TaskStructureTests::checkForTreenessTest()
{
    QFETCH( TaskList, tasks );
    QFETCH( bool, directed );

    QCOMPARE( Task::checkForTreeness( tasks ), directed );
}

void TaskStructureTests::mergeTaskListsTest_data()
{
    QTest::addColumn<TaskList>( "old" );
    QTest::addColumn<TaskList>( "newTasks" );
    QTest::addColumn<TaskList>( "merged" );

    Q_FOREACH( const QDomElement& testcase,
               TestHelpers::retrieveTestCases( ":/mergeTaskListsTest/Data", "mergeTaskListsTest" ) ) {
        QString name = testcase.attribute( "name" );

        QList<QDomElement> elements;
        elements << testcase.firstChildElement( Task::taskListTagName() );
        elements << ( elements.first() ).nextSiblingElement( Task::taskListTagName() );
        elements << ( elements.at( 1 ) ).nextSiblingElement( Task::taskListTagName() );
        bool oldFound = false, newFound = false, mergedFound = false;
        TaskList old, newTasks, merged;
        Q_FOREACH( const QDomElement& element, elements ) {
            QString arg = element.attribute( "arg" );
            TaskList tasks = Task::readTasksElement( element, CHARM_DATABASE_VERSION );
            if ( arg == "old" ) {
                old = tasks;
                oldFound = true;
            } else if ( arg == "new" ) {
                newTasks = tasks;
                newFound = true;
            } else if ( arg == "merged" ) {
                merged = tasks;
                qSort( merged.begin(), merged.end(), Task::lowerTaskId );
                mergedFound = true;
            } else {
                QFAIL( "invalid XML structure in input data" );
            }
        }
        QVERIFY( oldFound );
        QVERIFY( newFound );
        QVERIFY( mergedFound );

        QTest::newRow( name.toLocal8Bit() ) << old << newTasks << merged;
        qDebug() << "Added test case" << name;
    }
}

void TaskStructureTests::mergeTaskListsTest()
{
    QFETCH( TaskList, old );
    QFETCH( TaskList, newTasks );
    QFETCH( TaskList, merged );

    TaskListMerger merger;
    merger.setOldTasks( old );
    merger.setNewTasks( newTasks );

    TaskList result = merger.mergedTaskList();
    qSort( result.begin(), result.end(), Task::lowerTaskId );
    if ( result != merged ) {
        qDebug() << "Test failed";
        qDebug() << "Merge Result:";
        dumpTaskList( result );
        qDebug() << "Expected Merge Result:";
        dumpTaskList( merged );
    }

    QCOMPARE( result, merged );
}

QTEST_MAIN( TaskStructureTests )

#include "moc_TaskStructureTests.cpp"


