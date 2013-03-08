#ifndef CHARMDATAMODELTESTS_H
#define CHARMDATAMODELTESTS_H

#include <QObject>

class CharmDataModel;

class CharmDataModelTests : public QObject
{
    Q_OBJECT

public:
    CharmDataModelTests();

private slots:
    void initTestCase();
    void createAndDestroyTest();
    void addAndRemoveTasksTest();
    void modifyTaskTest();
    void cleanupTestCase();

private:
    CharmDataModel* m_referenceModel;
};

#endif
