#include <QtTest/QtTest>
#include <QSqlDatabase>
#include <QSqlDriver>

#include "SqlTransactionTests.h"

SqlTransactionTests::SqlTransactionTests()
    : QObject()
{
}

void SqlTransactionTests::testMySqlDriverRequirements()
{
    const char DriverName[] = "QMYSQL";
    QVERIFY( QSqlDatabase::isDriverAvailable( DriverName ) );
    QSqlDatabase db = QSqlDatabase::addDatabase( DriverName, "test-mysql.charm.kdab.net" );
    QSqlDriver* driver  = db.driver();
    // cannot check that yet, it can only be determined after login
    // QVERIFY( driver->hasFeature( QSqlDriver::Transactions ) );
}

void SqlTransactionTests::testSqLiteDriverRequirements()
{
    const char DriverName[] = "QSQLITE";
    QVERIFY( QSqlDatabase::isDriverAvailable( DriverName ) );
    QSqlDatabase db = QSqlDatabase::addDatabase( DriverName, "test-sqlite.charm.kdab.net" );
    QSqlDriver* driver  = db.driver();
    QVERIFY( driver->hasFeature( QSqlDriver::Transactions ) );
}

QTEST_MAIN( SqlTransactionTests )

#include "SqlTransactionTests.moc"
