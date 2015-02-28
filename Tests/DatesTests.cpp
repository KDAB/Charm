/*
  DatesTests.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Frank Osterfeld <frank.osterfeld@kdab.com>
  Author: Olivier JG <olivier.de.gaalon@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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

void DatesTests::testWeekDifference_data()
{
    QTest::addColumn<QDate>("from");
    QTest::addColumn<QDate>("to");
    QTest::addColumn<int>("weekDiff");
    QTest::newRow("2013/12/1 - 2013/12/30") << QDate(2013, 12, 1) << QDate(2013, 12, 30) << 5;
    QTest::newRow("2013/12/1 - 2013/12/31") << QDate(2013, 12, 1) << QDate(2013, 12, 31) << 5;
    QTest::newRow("2013/12/1 - 2014/1/3") << QDate(2013, 12, 1) << QDate(2014, 1, 3) << 5;
    QTest::newRow("2013/12/1 - 2013/1/3") << QDate(2013, 12, 1) << QDate(2013, 1, 3) << -47;
    QTest::newRow("2013/12/20 - 2013/1/5") << QDate(2013, 12, 20) << QDate(2014, 1, 5) << 2;
    QTest::newRow("2013/1/1 - 2014/1/1") << QDate(2013, 1, 1) << QDate(2014, 1, 1) << 52;
    QTest::newRow("2013/12/1 - 2050/4/3") << QDate(2013, 12, 1) << QDate(2050, 4, 3) << 1896;
    QTest::newRow("1994/12/1 - 2050/2/2") << QDate(1994, 12, 1) << QDate(2050, 2, 2) << 2879;
    QTest::newRow("2010/2/1 - 2010/3/1") << QDate(2010, 2, 1) << QDate(2010, 3, 1) << 4;
}
void DatesTests::testWeekDifference()
{
    QFETCH(QDate, from);
    QFETCH(QDate, to);
    QFETCH(int, weekDiff);
    QCOMPARE( Charm::weekDifference(from, to), weekDiff );
}

QTEST_MAIN( DatesTests )

#include "moc_DatesTests.cpp"
