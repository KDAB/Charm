#ifndef XMLSERIALIZATIONTESTS_H
#define XMLSERIALIZATIONTESTS_H

#include <QObject>

#include "Core/Task.h"

class XmlSerializationTests : public QObject
{
    Q_OBJECT

public:
    XmlSerializationTests();

private slots:
    void testEventSerialization();
    void testTaskSerialization();
    void testTaskListSerialization();
    void testQDateTimeToFromString();
    void testTaskExportImport();

private:
    TaskList tasksToTest();
};

#endif
