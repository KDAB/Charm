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
    void importExportTest();
    void importBenchmark();
    void exportBenchmark();
    void cleanupTestCase();

private:
    void importDatabase( const QString& filename );
};

#endif
