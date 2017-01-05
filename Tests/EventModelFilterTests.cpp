/*
  EventModelFilterTests.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2012-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Michel Boyer de la Giroday <michel.giroday@kdab.com>

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

#include "EventModelFilterTests.h"
#include "Charm/EventModelFilter.h"
#include "Core/CharmDataModel.h"
#include "Core/Event.h"
#include "Core/Task.h"

#include <QDateTime>
#include <QtTest/QtTest>

EventModelFilterTests::EventModelFilterTests()
    : QObject()
{
}

void EventModelFilterTests::initTestCase()
{
    // set up a model
    m_referenceModel = new CharmDataModel;

    // add tasks
    Task task1( 1000, QStringLiteral("Task 1") );
    TaskList task;
    task << task1;
    m_referenceModel->setAllTasks( task );
    m_eventModelFilter = new EventModelFilter( m_referenceModel, this );

    TimeSpans spans;
    m_thisYearSpan = spans.thisYear();
    m_theMonthBeforeLastSpan = spans.theMonthBeforeLast();
    m_lastMonthSpan = spans.lastMonth();
    m_thisMonthSpan = spans.thisMonth();
    m_theWeekBeforeLastSpan = spans.theWeekBeforeLast();
    m_lastWeekSpan = spans.lastWeek();
    m_thisWeekSpan = spans.thisWeek();
    m_dayBeforeYesterdaySpan = spans.dayBeforeYesterday();
    m_yesterdaySpan = spans.yesterday();
    m_todaySpan = spans.today();
    NamedTimeSpan allEvents = {
        tr( "Ever" ),
        TimeSpan( QDate::currentDate().addYears( -200 ),
                  QDate::currentDate().addYears( +200 ) ),
        Range
    };
    m_everSpan = allEvents;
}

void EventModelFilterTests::checkYearsFilter()
{
    Event event1, event2;
    QDateTime time = QDateTime::currentDateTime();
    time.setDate( m_thisYearSpan.timespan.first  );
    // Last year
    event1.setId( 1 );
    event1.setComment( QStringLiteral("event1") );
    event1.setTaskId( 1000 );
    event1.setStartDateTime( time.addYears( -1 ) );
    event1.setEndDateTime( time.addYears( -1 ).addSecs( 3600 ) );
    // This year
    event2.setId( 2 );
    event2.setComment( QStringLiteral("event2") );
    event2.setTaskId( 1000 );
    event2.setStartDateTime( time );
    event2.setEndDateTime( time.addSecs( 3600 ) );

    EventList events;
    events << event1 << event2;
    m_referenceModel->setAllEvents( events );

    // Last year
    m_eventModelFilter->setFilterStartDate( m_thisYearSpan.timespan.first.addYears( -1 ) );
    m_eventModelFilter->setFilterEndDate( m_thisYearSpan.timespan.second.addYears( -1 ) );
    QVERIFY( m_eventModelFilter->events().count() == 1 );

    // This year
    m_eventModelFilter->setFilterStartDate( m_thisYearSpan.timespan.first );
    m_eventModelFilter->setFilterEndDate( m_thisYearSpan.timespan.second );
    QVERIFY( m_eventModelFilter->events().count() == 1 );

    m_referenceModel->clearEvents();
}

void EventModelFilterTests::checkMonthsFilter()
{
    m_referenceModel->clearEvents();
    Event event1, event2, event3;
    QDateTime time = QDateTime::currentDateTime();
    time.setDate( m_theMonthBeforeLastSpan.timespan.first );
    event1.setId( 1 );
    event1.setComment( QStringLiteral("event1") );
    event1.setTaskId( 1000 );
    event1.setStartDateTime( time );
    event1.setEndDateTime( time.addSecs( 3600 ) );

    time.setDate( m_lastMonthSpan.timespan.first );
    event2.setId( 2 );
    event2.setComment( QStringLiteral("event2") );
    event2.setTaskId( 1000 );
    event2.setStartDateTime( time );
    event2.setEndDateTime( time.addSecs( 3600 ) );

    time.setDate( m_thisMonthSpan.timespan.first );
    event3.setId( 3 );
    event3.setComment( QStringLiteral("event3") );
    event3.setTaskId( 1000 );
    event3.setStartDateTime( time );
    event3.setEndDateTime( time.addSecs( 3600 ) );

    EventList events;
    events << event1 << event2 << event3;
    m_referenceModel->setAllEvents( events );

    // The month before last month
    m_eventModelFilter->setFilterStartDate( m_theMonthBeforeLastSpan.timespan.first );
    m_eventModelFilter->setFilterEndDate( m_theMonthBeforeLastSpan.timespan.second );
    m_eventModelFilter->events();
    QVERIFY( m_eventModelFilter->events().count() == 1 );
    // Last month
    m_eventModelFilter->setFilterStartDate( m_lastMonthSpan.timespan.first );
    m_eventModelFilter->setFilterEndDate( m_lastMonthSpan.timespan.second );
    m_eventModelFilter->events();
    QVERIFY( m_eventModelFilter->events().count() == 1 );
    // This month
    m_eventModelFilter->setFilterStartDate( m_thisMonthSpan.timespan.first );
    m_eventModelFilter->setFilterEndDate( m_thisMonthSpan.timespan.second );
    m_eventModelFilter->events();
    QVERIFY( m_eventModelFilter->events().count() == 1 );

    m_referenceModel->clearEvents();
}

void EventModelFilterTests::checkWeeksFilter()
{
    Event event1, event2, event3;
    QDateTime time = QDateTime::currentDateTime();
    time.setDate( m_theWeekBeforeLastSpan.timespan.first );
    event1.setId( 1 );
    event1.setComment( QStringLiteral("event1") );
    event1.setTaskId( 1000 );
    event1.setStartDateTime( time );
    event1.setEndDateTime( time.addSecs( 3600 ) );

    time.setDate( m_lastWeekSpan.timespan.first );
    event2.setId( 2 );
    event2.setComment( QStringLiteral("event2") );
    event2.setTaskId( 1000 );
    event2.setStartDateTime( time );
    event2.setEndDateTime( time.addSecs( 3600 ) );

    time.setDate( m_thisWeekSpan.timespan.first );
    event3.setId( 3 );
    event3.setComment( QStringLiteral("event3") );
    event3.setTaskId( 1000 );
    event3.setStartDateTime( time );
    event3.setEndDateTime( time.addSecs( 3600 ) );

    EventList events;
    events << event1 << event2 << event3;
    m_referenceModel->setAllEvents( events );

    // The week before last week
    m_eventModelFilter->setFilterStartDate( m_theWeekBeforeLastSpan.timespan.first );
    m_eventModelFilter->setFilterEndDate( m_theWeekBeforeLastSpan.timespan.second );
    m_eventModelFilter->events();
    QVERIFY( m_eventModelFilter->events().count() == 1 );
    // Last week
    m_eventModelFilter->setFilterStartDate( m_lastWeekSpan.timespan.first );
    m_eventModelFilter->setFilterEndDate( m_lastWeekSpan.timespan.second );
    m_eventModelFilter->events();
    QVERIFY( m_eventModelFilter->events().count() == 1 );
    // This week
    m_eventModelFilter->setFilterStartDate( m_thisWeekSpan.timespan.first );
    m_eventModelFilter->setFilterEndDate( m_thisWeekSpan.timespan.second );
    m_eventModelFilter->events();
    QVERIFY( m_eventModelFilter->events().count() == 1 );

    m_referenceModel->clearEvents();
}

void EventModelFilterTests::checkDaysFilter()
{
    Event event1, event2, event3;
    QDateTime time = QDateTime::currentDateTime();
    time.setDate( m_dayBeforeYesterdaySpan.timespan.first );
    event1.setId( 1 );
    event1.setComment( QStringLiteral("event1") );
    event1.setTaskId( 1000 );
    event1.setStartDateTime( time );
    event1.setEndDateTime( time.addSecs( 3600 ) );

    time.setDate( m_yesterdaySpan.timespan.first );
    event2.setId( 2 );
    event2.setComment( QStringLiteral("event2") );
    event2.setTaskId( 1000 );
    event2.setStartDateTime( time );
    event2.setEndDateTime( time.addSecs( 3600 ) );

    time.setDate( m_todaySpan.timespan.first );
    event3.setId( 3 );
    event3.setComment( QStringLiteral("event3") );
    event3.setTaskId( 1000 );
    event3.setStartDateTime( time );
    event3.setEndDateTime( time.addSecs( 3600 ) );

    EventList events;
    events << event1 << event2 << event3;
    m_referenceModel->setAllEvents( events );

    // The day before yesterday
    m_eventModelFilter->setFilterStartDate( m_dayBeforeYesterdaySpan.timespan.first );
    m_eventModelFilter->setFilterEndDate( m_dayBeforeYesterdaySpan.timespan.second );
    QVERIFY( m_eventModelFilter->events().count() == 1 );
    // Yesterday
    m_eventModelFilter->setFilterStartDate( m_yesterdaySpan.timespan.first );
    m_eventModelFilter->setFilterEndDate( m_yesterdaySpan.timespan.second );
    QVERIFY( m_eventModelFilter->events().count() == 1 );
    // Today
    m_eventModelFilter->setFilterStartDate( m_todaySpan.timespan.first );
    m_eventModelFilter->setFilterEndDate( m_todaySpan.timespan.second );
    QVERIFY( m_eventModelFilter->events().count() == 1 );

    // Ever (all events)
    m_eventModelFilter->setFilterStartDate( m_everSpan.timespan.first );
    m_eventModelFilter->setFilterEndDate( m_everSpan.timespan.second );
    m_eventModelFilter->events();
    QVERIFY( m_eventModelFilter->events().count() == 3 );

    m_referenceModel->clearEvents();
}

void EventModelFilterTests::checkEventSpanOver2Weeks()
{
    Event event1;
    QDateTime time = QDateTime::currentDateTime();
    time.setDate( m_theWeekBeforeLastSpan.timespan.first );
    event1.setId( 1 );
    event1.setComment( QStringLiteral("event1") );
    event1.setTaskId( 1000 );
    event1.setStartDateTime( time );
    event1.setEndDateTime( time.addDays( 8 ) );

    EventList events;
    events << event1;
    m_referenceModel->setAllEvents( events );

    // Check the week before last week and last week
    m_eventModelFilter->setFilterStartDate( m_theWeekBeforeLastSpan.timespan.first );
    m_eventModelFilter->setFilterEndDate( m_theWeekBeforeLastSpan.timespan.second );
    m_eventModelFilter->events();
    QVERIFY( m_eventModelFilter->events().count() == 1 );

    m_eventModelFilter->setFilterStartDate( m_lastWeekSpan.timespan.first );
    m_eventModelFilter->setFilterEndDate( m_lastWeekSpan.timespan.second );
    m_eventModelFilter->events();
    QVERIFY( m_eventModelFilter->events().count() == 1 );

    m_referenceModel->clearEvents();
}

void EventModelFilterTests::checkEventSpanOver2Days()
{
    Event event1;
    QDateTime time = QDateTime::currentDateTime();
    time.setDate( m_dayBeforeYesterdaySpan.timespan.first );
    event1.setId( 1 );
    event1.setComment( QStringLiteral("event1") );
    event1.setTaskId( 1000 );
    event1.setStartDateTime( time );
    event1.setEndDateTime( time.addDays( 1 ) );

    EventList events;
    events << event1;
    m_referenceModel->setAllEvents( events );

    // check the day before yesterday and yesterday
    m_eventModelFilter->setFilterStartDate( m_dayBeforeYesterdaySpan.timespan.first );
    m_eventModelFilter->setFilterEndDate( m_dayBeforeYesterdaySpan.timespan.second );
    QVERIFY( m_eventModelFilter->events().count() == 1 );

    m_eventModelFilter->setFilterStartDate( m_yesterdaySpan.timespan.first );
    m_eventModelFilter->setFilterEndDate( m_yesterdaySpan.timespan.second );
    QVERIFY( m_eventModelFilter->events().count() == 1 );

    m_referenceModel->clearEvents();
}

QTEST_MAIN( EventModelFilterTests )

#include "moc_EventModelFilterTests.cpp"
