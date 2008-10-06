#include <QtDebug>
#include <QtTest/QtTest>
#include <QDir>
#include <QFileInfo>

#include "Core/Task.h"
#include "Core/TaskListMerger.h"
#include "Core/CharmConstants.h"
#include "Core/CharmExceptions.h"

#include "TaskStructureTests.h"

TaskStructureTests::TaskStructureTests()
    : QObject()
{
}

QList<QDomElement> TaskStructureTests::retrieveTestCases( QString path, QString type )
{
    const QString tagName( "testcase" );
    QStringList filenamePatterns;
    filenamePatterns << "*.xml";

    QDir dataDir( path );
    if ( !dataDir.exists() ) {
        throw CharmException( "path to test case data does not exist" );
    }

    QFileInfoList dataSets = dataDir.entryInfoList( filenamePatterns, QDir::Files, QDir::Name );

    QList<QDomElement> result;
    Q_FOREACH( QFileInfo fileinfo, dataSets ) {
        QDomDocument doc( "charmtests" );
        QFile file( fileinfo.filePath() );
        if ( ! file.open( QIODevice::ReadOnly ) ) {
            throw CharmException( "unable to open input file" );
        }

        if ( !doc.setContent( &file ) ) {
            throw CharmException( "invalid DOM document, cannot load" );
        }

        QDomElement root = doc.firstChildElement();
        if ( root.tagName() != "testcases" ) {
            throw CharmException( "root element (testcases) not found" );
        }

        qDebug() << "Loading test cases from" << file.fileName();

        for ( QDomElement child = root.firstChildElement( tagName ); !child.isNull();
              child = child.nextSiblingElement( tagName ) ) {
            if ( child.attribute( "type" ) == type ) {
                result << child;
            }
        }
    }
    return result;
}

bool TaskStructureTests::attribute( const QString& name, const QDomElement& element )
{
    QString text = element.attribute( "expectedResult" );
    if ( text != "false" && text != "true" ) {
        throw CharmException( "attribute does not represent a boolean" );
    }
    return ( text == "true" );
}

void TaskStructureTests::checkForUniqueTaskIdsTest_data()
{
    QTest::addColumn<TaskList>( "tasks" );
    QTest::addColumn<bool>( "unique" );

    Q_FOREACH( QDomElement testcase,
               retrieveTestCases( ":/checkForUniqueTaskIdsTest/Data", "checkForUniqueTaskIdsTest" ) ) {
        QString name = testcase.attribute( "name" );
        bool expectedResult = attribute( "expectedResult", testcase );
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

    Q_FOREACH( QDomElement testcase,
               retrieveTestCases( ":/checkForTreenessTest/Data", "checkForTreenessTest" ) ) {
        QString name = testcase.attribute( "name" );
        bool expectedResult = attribute( "expectedResult", testcase );
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

    Q_FOREACH( QDomElement testcase,
               retrieveTestCases( ":/mergeTaskListsTest/Data", "mergeTaskListsTest" ) ) {
        QString name = testcase.attribute( "name" );

        QList<QDomElement> elements;
        elements << testcase.firstChildElement( Task::taskListTagName() );
        elements << ( elements.first() ).nextSiblingElement( Task::taskListTagName() );
        elements << ( elements.at( 1 ) ).nextSiblingElement( Task::taskListTagName() );
        bool oldFound = false, newFound = false, mergedFound = false;
        TaskList old, newTasks, merged;
        Q_FOREACH( QDomElement element, elements ) {
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

#include "TaskStructureTests.moc"


