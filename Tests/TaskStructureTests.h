#ifndef TASKSTRUCTURETESTS_H
#define TASKSTRUCTURETESTS_H

#include <QObject>

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

    void mergeTaskListsTest();

};

#endif
