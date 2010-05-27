#ifndef IMPORTEXPORTTESTS_H
#define IMPORTEXPORTTESTS_H

#include "TestApplication.h"

class ImportExportTests : public TestApplication
{
    Q_OBJECT

public:
    ImportExportTests();

private slots:
    void initTestCase();
        // void importExportTest_data();
    void importExportTest();
    void cleanupTestCase();
};

#endif
