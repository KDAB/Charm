#ifndef BACKENDINTEGRATIONTESTS_H
#define BACKENDINTEGRATIONTESTS_H

#include "TestApplication.h"
#include "Core/Task.h"

class BackendIntegrationTests : public TestApplication
{
    Q_OBJECT

public:
    BackendIntegrationTests();

private slots:
    void initTestCase ();

    void initialValuesTest();

    void simpleCreateModifyDeleteTaskTest();

    void biggerCreateModifyDeleteTaskTest();

    void cleanupTestCase ();

private:
    const TaskList& referenceTasks();
    // returns true if both lists contain the same tasks, even if not
    // in the same order:
    bool contentsEqual( const TaskList& list1, const TaskList& list2 );
};

#endif
