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

QTEST_MAIN( DatesTests )

#include "DatesTests.moc"
