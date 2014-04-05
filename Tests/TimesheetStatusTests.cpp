#include "TimesheetStatusTests.h"

#include <QDateTime>
#include <QtDebug>
#include <QtTest/QtTest>

TimesheetStatusTests::TimesheetStatusTests( QObject *parent )
    : TimesheetStatus( parent )
{
}

void TimesheetStatusTests::testParseTimesheetStatus()
{
    QFile payload(":/timesheetStatusTest/Data/test-timesheets.xml");
    payload.open(QFile::ReadOnly);
    Q_ASSERT(payload.isOpen());
    QVERIFY(parse(payload.readAll()));
    payload.close();

    const QDate currentDate = QDate::currentDate();
    const QDate lastWeekDate = currentDate.addDays(-currentDate.dayOfWeek());
    const int lastWeekYear = lastWeekDate.year();
    const int lastWeekNumber = lastWeekDate.weekNumber();

    QVERIFY(missingTimesheetYear() == lastWeekYear
        && missingTimesheetWeek() == lastWeekNumber);
}

QTEST_MAIN( TimesheetStatusTests )

#include "moc_TimesheetStatusTests.cpp"
