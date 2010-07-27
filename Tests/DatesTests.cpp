#include "DatesTests.h"
#include "Core/Dates.h"

#include <QtTest/QtTest>


void DatesTests::testDates()
{
    QCOMPARE( Charm::dateByWeekNumberAndWeekDay( 2009, 53, Qt::Friday ), QDate( 2010, 1, 1 ) );
    QCOMPARE( Charm::dateByWeekNumberAndWeekDay( 2009, 52, Qt::Monday ), QDate( 2009, 12, 21 ) );
    QCOMPARE( Charm::dateByWeekNumberAndWeekDay( 2010, 1, Qt::Wednesday ), QDate( 2010, 1, 6 ) );
    QCOMPARE( Charm::dateByWeekNumberAndWeekDay( 2010, 1, Qt::Wednesday ), QDate( 2010, 1, 6 ) );
    QCOMPARE( Charm::dateByWeekNumberAndWeekDay( 2010, 30, Qt::Tuesday ), QDate( 2010, 7, 27 ) );
    QCOMPARE( Charm::dateByWeekNumberAndWeekDay( 2010, 52, Qt::Saturday ), QDate( 2011, 1, 1 ) );
    QCOMPARE( Charm::dateByWeekNumberAndWeekDay( 2011, 1, Qt::Monday ), QDate( 2011, 1, 3 ) );
}


QTEST_MAIN( DatesTests )

#include "DatesTests.moc"
