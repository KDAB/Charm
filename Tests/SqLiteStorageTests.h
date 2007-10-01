#ifndef SQLITESTORAGETEST_H
#define SQLITESTORAGETEST_H

#include <QObject>

#include <Configuration.h>

#include <StorageInterface.h>

class SqLiteStorageTests : public QObject
{
    Q_OBJECT
public:
    SqLiteStorageTests();

private:
    StorageInterface* m_storage;
    Configuration m_configuration;
    QString m_localPath;

private slots:
    void initTestCase ();

    void connectAndCreateDatabaseTest();

    void makeModifyDeleteInstallationTest();

    void makeModifyDeleteUserTest();

    void makeModifyDeleteTasksTest();

    void makeModifyDeleteEventsTest();

    void addDeleteSubscriptionsTest();

    void setGetMetaDataTest();

    void cleanupTestCase();
};

#endif
