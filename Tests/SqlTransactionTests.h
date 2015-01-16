#ifndef SQLTRANSACTIONTESTS_H
#define SQLTRANSACTIONTESTS_H

#include <QObject>

#include "Core/MySqlStorage.h"

class SqlTransactionTests : public QObject
{
    Q_OBJECT

public:
    SqlTransactionTests();

private slots:
    void testMySqlDriverRequirements();
    void testSqLiteDriverRequirements();
#if 0
    void testMySqlTransactionRollback();
    void testMySqlTransactionCommit();
    void testMySqlNestedTransactions();
#endif

private:
    MySqlStorage prepareMySqlStorage();
};


#endif // SQLTRANSACTIONTESTS_H
