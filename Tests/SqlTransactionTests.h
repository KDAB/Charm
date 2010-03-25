#ifndef SQLTRANSACTIONTESTS_H
#define SQLTRANSACTIONTESTS_H

#include <QObject>

class SqlTransactionTests : public QObject
{
    Q_OBJECT

public:
    SqlTransactionTests();

private slots:
    void testMySqlDriverRequirements();
    void testSqLiteDriverRequirements();
};


#endif // SQLTRANSACTIONTESTS_H
