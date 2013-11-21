#include <QtTest/QtTest>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlDriver>
#include <QSqlError>

#include "Core/SqlRaiiTransactor.h"

#include "SqlTransactionTests.h"

SqlTransactionTests::SqlTransactionTests()
    : QObject()
{
}

void SqlTransactionTests::testMySqlDriverRequirements()
{
    const char DriverName[] = "QMYSQL";
    QVERIFY( QSqlDatabase::isDriverAvailable( DriverName ) );
    QSqlDatabase db = QSqlDatabase::addDatabase( DriverName, "test-mysql.charm.kdab.com" );
}

void SqlTransactionTests::testSqLiteDriverRequirements()
{
    const char DriverName[] = "QSQLITE";
    QVERIFY( QSqlDatabase::isDriverAvailable( DriverName ) );
    QSqlDatabase db = QSqlDatabase::addDatabase( DriverName, "test-sqlite.charm.kdab.com" );
    QSqlDriver* driver  = db.driver();
    QVERIFY( driver->hasFeature( QSqlDriver::Transactions ) );
}

MySqlStorage SqlTransactionTests::prepareMySqlStorage()
{
    MySqlStorage storage;
    MySqlStorage::Parameters parameters = MySqlStorage::parseParameterEnvironmentVariable();
    storage.configure( parameters );
    return storage;
}

void SqlTransactionTests::testMySqlTransactionRollback()
{
    MySqlStorage storage = prepareMySqlStorage();
    QVERIFY( storage.database().open() );

    QSqlDriver* driver  = storage.database().driver();
    QVERIFY( driver->hasFeature( QSqlDriver::Transactions ) );

    QList<Task> tasksBefore = storage.getAllTasks();
    QVERIFY( ! tasksBefore.isEmpty() );
    Task first = tasksBefore.first();
    // test a simple transaction that is completed and committed:
    {
        SqlRaiiTransactor transactor( storage.database() );
        QSqlQuery query( storage.database() );
        query.prepare("DELETE from Tasks where id=:id");
        query.bindValue( "id", first.id() );
        QVERIFY( storage.runQuery( query ) );
    } // this transaction was NOT commited
    QList<Task> tasksAfter = storage.getAllTasks();
    QVERIFY( ! tasksAfter.isEmpty() );
    QVERIFY( tasksBefore == tasksAfter );
}

void SqlTransactionTests::testMySqlTransactionCommit()
{
    MySqlStorage storage = prepareMySqlStorage();
    QVERIFY( storage.database().open() );

    QList<Task> tasksBefore = storage.getAllTasks();
    QVERIFY( ! tasksBefore.isEmpty() );
    Task first = tasksBefore.takeFirst();
    // test a simple transaction that is completed and committed:
    {
        SqlRaiiTransactor transactor( storage.database() );
        QSqlQuery query( storage.database() );
        query.prepare("DELETE from Tasks where id=:id");
        query.bindValue( "id", first.id() );
        QVERIFY( storage.runQuery( query ) );
        transactor.commit();
    } // this transaction WAS commited
    QList<Task> tasksAfter = storage.getAllTasks();
    QVERIFY( ! tasksAfter.isEmpty() );
    QVERIFY( tasksBefore == tasksAfter );
}

// this test more or less documents the behaviour of the mysql driver which allows nested transactions, which fail, without
// reporting an error
void SqlTransactionTests::testMySqlNestedTransactions()
{
    MySqlStorage storage = prepareMySqlStorage();
    QVERIFY( storage.database().open() );

    QList<Task> tasksBefore = storage.getAllTasks();
    QVERIFY( ! tasksBefore.isEmpty() );
    Task first = tasksBefore.takeFirst();
    // test a simple transaction that is completed and committed:
    {
        SqlRaiiTransactor transactor( storage.database() );
        QSqlQuery query( storage.database() );
        query.prepare("DELETE from Tasks where id=:id");
        query.bindValue( "id", first.id() );
        QVERIFY( storage.runQuery( query ) );
        {   // now before the first transaction is commited and done, a second one is started
            // this should throw an exception
            SqlRaiiTransactor transactor2( storage.database() );
            QSqlError error = storage.database().lastError();
            // QFAIL( "I should not get here." );
            transactor2.commit();
        }
        QSqlError error1 = storage.database().lastError();
        transactor.commit();
        QSqlError error2 = storage.database().lastError();
        QFAIL( "I should not get here." );
    }
}

QTEST_MAIN( SqlTransactionTests )

#include "moc_SqlTransactionTests.cpp"
