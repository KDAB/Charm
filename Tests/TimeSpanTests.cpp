#include "TimeSpanTests.h"
#include "Core/TimeSpans.h"
#include <QtTest/QtTest>

#include <QDebug>

void TimeSpanTests::testTimeSpans()
{
    const QDate dec31 = QDate( 2012, 2, 1 );
    const QDate jan1 = QDate( 2012, 1, 1 );
    const QDate jan30 = QDate( 2012, 1, 30 );
    const QDate jan31 = QDate( 2012, 1, 31 );
    const QDate feb1 = QDate( 2012, 2, 1 );
    const QDate feb2 = QDate( 2012, 2, 2 );
    const QDate feb29 = QDate( 2012, 2, 29 );
    const QDate mar1 = QDate( 2012, 3, 1 );
    const TimeSpans spans( feb1 );
    QVERIFY( spans.today().contains( feb1 ) );
    QVERIFY( !spans.today().contains( jan31 ) );
    QVERIFY( !spans.today().contains( feb2 ) );
    QVERIFY( spans.yesterday().contains( jan31 ) );
    QVERIFY( !spans.yesterday().contains( feb1 ) );
    QVERIFY( spans.dayBeforeYesterday().contains( jan30 ) );
    QVERIFY( !spans.dayBeforeYesterday().contains( jan31 ) );
    QCOMPARE( spans.thisWeek().timespan.first, QDate( 2012, 1, 30 ) );
    QCOMPARE( spans.thisWeek().timespan.second, QDate( 2012, 2, 6 ) );
    QCOMPARE( spans.lastWeek().timespan.first, QDate( 2012, 1, 23 ) );
    QCOMPARE( spans.lastWeek().timespan.second, QDate( 2012, 1, 30 ) );
    QCOMPARE( spans.theWeekBeforeLast().timespan.first, QDate( 2012, 1, 16 ) );
    QCOMPARE( spans.theWeekBeforeLast().timespan.second, QDate( 2012, 1, 23 ) );
    QCOMPARE( spans.thisMonth().timespan.first, feb1 );
    QCOMPARE( spans.thisMonth().timespan.second, mar1 );
    QVERIFY( spans.thisMonth().contains( feb29 ) );
    QVERIFY( spans.lastMonth().contains( jan1 ) );
    QVERIFY( spans.lastMonth().contains( jan31 ) );
    QVERIFY( !spans.lastMonth().contains( dec31 ) );
    QVERIFY( !spans.lastMonth().contains( feb1 ) );
}

QTEST_MAIN( TimeSpanTests )

#include "moc_TimeSpanTests.cpp"
