#ifndef TIMESHEETPROCESSORTESTS_H
#define TIMESHEETPROCESSORTESTS_H

#include <QObject>

class TimeSheetProcessorTests : public QObject
{
    Q_OBJECT

public:
    explicit TimeSheetProcessorTests();

private slots:
    void testAddRemoveTimeSheet();

private:
    int m_idTimeSheet;
    int m_adminId;
    QString m_reportPath;
};

#endif
