#ifndef TASKSTRUCTURETESTS_H
#define TASKSTRUCTURETESTS_H

#include <QObject>
#include <QList>
#include <QDomElement>

class TaskStructureTests : public QObject
{
    Q_OBJECT

public:
    TaskStructureTests();

private slots:

    void checkForUniqueTaskIdsTest_data();
    void checkForUniqueTaskIdsTest();

    void checkForTreenessTest_data();
    void checkForTreenessTest();

    void mergeTaskListsTest_data();
    void mergeTaskListsTest();

private:
    QList<QDomElement> retrieveTestCases( QString path, QString type );
};

#endif
