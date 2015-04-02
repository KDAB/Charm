/*
  SqlTransactionTests.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>

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

#include "SqlTransactionTests.h"

#include "Core/SqlRaiiTransactor.h"

#include <QtTest/QtTest>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlDriver>
#include <QSqlError>

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

#if 0 // old broken tests, commented so far to let us test some other part of the TimeSheetProcessor
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
    } // this transaction was NOT committed
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
    } // this transaction WAS committed
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
        {   // now before the first transaction is committed and done, a second one is started
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
#endif
QTEST_MAIN( SqlTransactionTests )

#include "moc_SqlTransactionTests.cpp"
