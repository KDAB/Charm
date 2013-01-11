#include "DatesTests.h"
#include "Core/Dates.h"

#include <QtTest/QtTest>


void DatesTests::testDateByWeekNumberAndWorkDay()
{
    QCOMPARE( Charm::dateByWeekNumberAndWeekDay( 2009, 53, Qt::Friday ), QDate( 2010, 1, 1 ) );
    QCOMPARE( Charm::dateByWeekNumberAndWeekDay( 2009, 52, Qt::Monday ), QDate( 2009, 12, 21 ) );
    QCOMPARE( Charm::dateByWeekNumberAndWeekDay( 2010, 1, Qt::Wednesday ), QDate( 2010, 1, 6 ) );
    QCOMPARE( Charm::dateByWeekNumberAndWeekDay( 2010, 30, Qt::Tuesday ), QDate( 2010, 7, 27 ) );
    QCOMPARE( Charm::dateByWeekNumberAndWeekDay( 2010, 52, Qt::Saturday ), QDate( 2011, 1, 1 ) );
    QCOMPARE( Charm::dateByWeekNumberAndWeekDay( 2011, 1, Qt::Monday ), QDate( 2011, 1, 3 ) );
    QCOMPARE( Charm::dateByWeekNumberAndWeekDay( 2012, 52, Qt::Sunday ), QDate( 2012, 12, 30 ) );
    QCOMPARE( Charm::dateByWeekNumberAndWeekDay( 2013, 1, Qt::Monday ), QDate( 2012, 12, 31 ) );
}

void DatesTests::testWeekDayInWeekOf()
{
    QCOMPARE( Charm::weekDayInWeekOf( Qt::Monday, QDate( 2011, 12, 31 ) ), QDate( 2011, 12, 26 ) );
    QCOMPARE( Charm::weekDayInWeekOf( Qt::Tuesday, QDate( 2011, 12, 31 ) ), QDate( 2011, 12, 27 ) );
    QCOMPARE( Charm::weekDayInWeekOf( Qt::Wednesday, QDate( 2011, 12, 31 ) ), QDate( 2011, 12, 28 ) );
    QCOMPARE( Charm::weekDayInWeekOf( Qt::Thursday, QDate( 2011, 12, 31 ) ), QDate( 2011, 12, 29 ) );
    QCOMPARE( Charm::weekDayInWeekOf( Qt::Friday, QDate( 2011, 12, 31 ) ), QDate( 2011, 12, 30 ) );
    QCOMPARE( Charm::weekDayInWeekOf( Qt::Saturday, QDate( 2011, 12, 31 ) ), QDate( 2011, 12, 31 ) );
    QCOMPARE( Charm::weekDayInWeekOf( Qt::Sunday, QDate( 2011, 12, 31 ) ), QDate( 2012, 1, 1 ) );
}

void DatesTests::testNumberOfWeeksInYear_data()
{
    QTest::addColumn<int>("year");
    QTest::addColumn<int>("numWeeks");
    QTest::newRow("Weeks in 2010") << 2010 << 52;
    QTest::newRow("Weeks in 2011") << 2011 << 52;
    QTest::newRow("Weeks in 2012") << 2012 << 52;
    QTest::newRow("Weeks in 2013") << 2013 << 52;
    QTest::newRow("Weeks in 2014") << 2014 << 52;
    QTest::newRow("Weeks in 2015") << 2015 << 53;
    QTest::newRow("Weeks in 2016") << 2016 << 52;
    QTest::newRow("Weeks in 2017") << 2017 << 52;
    QTest::newRow("Weeks in 2018") << 2018 << 52;
    QTest::newRow("Weeks in 2019") << 2019 << 52;
    QTest::newRow("Weeks in 2020") << 2020 << 53;
    QTest::newRow("Weeks in 2021") << 2021 << 52;
    QTest::newRow("Weeks in 2022") << 2022 << 52;
    QTest::newRow("Weeks in 2023") << 2023 << 52;
    QTest::newRow("Weeks in 2024") << 2024 << 52;
    QTest::newRow("Weeks in 2025") << 2025 << 52;
    QTest::newRow("Weeks in 2026") << 2026 << 53;
    QTest::newRow("Weeks in 2027") << 2027 << 52;
    QTest::newRow("Weeks in 2028") << 2028 << 52;
    QTest::newRow("Weeks in 2029") << 2029 << 52;
    QTest::newRow("Weeks in 2030") << 2030 << 52;
}
void DatesTests::testNumberOfWeeksInYear()
{
    QFETCH(int, year);
    QFETCH(int, numWeeks);
    QCOMPARE( Charm::numberOfWeeksInYear(year), numWeeks );
}

QTEST_MAIN( DatesTests )

#include "DatesTests.moc"
