/*
  TimeSpans.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2007-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>
  Author: Frank Osterfeld <frank.osterfeld@kdab.com>

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

#include "TimeSpans.h"

#include <QtDebug>
#include <QTime>

TimeSpans::TimeSpans(const QDate &today)
{
    m_today.name = tr( "Today" );
    m_today.timespan =
        TimeSpan( today,
                  today.addDays( 1 ) );
    m_today.timeSpanType = Day;

    m_yesterday.name = tr( "Yesterday" );
    m_yesterday.timespan =
        TimeSpan( m_today.timespan.first.addDays( -1 ),
                  m_today.timespan.second.addDays( -1 ) );
    m_yesterday.timeSpanType = Day;

    m_dayBeforeYesterday.name = tr( "The Day Before Yesterday" );
    m_dayBeforeYesterday.timespan =
        TimeSpan( m_today.timespan.first.addDays( -2 ),
                  m_today.timespan.second.addDays( -2 ) );
    m_dayBeforeYesterday.timeSpanType = Day;
    m_thisWeek.name = tr( "This Week" );
    m_thisWeek.timespan =
        TimeSpan( today.addDays( - today.dayOfWeek() + 1 ),
                  today.addDays( 7 - today.dayOfWeek() + 1 ) );
    m_thisWeek.timeSpanType = Week;

    m_lastWeek.name = tr( "Last Week" );
    m_lastWeek.timespan =
        TimeSpan( m_thisWeek.timespan.first.addDays( -7 ),
                  m_thisWeek.timespan.second.addDays( -7 ) );
    m_lastWeek.timeSpanType = Week;

    m_theWeekBeforeLast.name = tr( "The Week Before Last Week" );
    m_theWeekBeforeLast.timespan =
        TimeSpan( m_thisWeek.timespan.first.addDays( -14 ),
                  m_thisWeek.timespan.second.addDays( -14 ) );
    m_theWeekBeforeLast.timeSpanType = Week;

    m_3WeeksAgo.name = tr( "3 Weeks Ago" );
    m_3WeeksAgo.timespan =
        TimeSpan( m_thisWeek.timespan.first.addDays( -21 ),
                  m_thisWeek.timespan.second.addDays( -21 ) );
    m_3WeeksAgo.timeSpanType = Week;

    m_thisMonth.name = tr( "This Month" );
    m_thisMonth.timespan =
        TimeSpan( today.addDays( - today.day() + 1 ),
                  today.addDays( today.daysInMonth() - today.day() + 1) );
    m_thisMonth.timeSpanType = Month;

    m_lastMonth.name = tr( "Last Month" );
    m_lastMonth.timespan =
        TimeSpan( m_thisMonth.timespan.first.addMonths( -1 ),
                  m_thisMonth.timespan.second.addMonths( -1 ) );
    m_lastMonth.timeSpanType = Month;

    m_theMonthBeforeLast.name = tr( "The Month Before Last Month" );
    m_theMonthBeforeLast.timespan =
        TimeSpan( m_thisMonth.timespan.first.addMonths( -2 ),
                  m_thisMonth.timespan.second.addMonths( -2 ) );
    m_theMonthBeforeLast.timeSpanType = Month;

    m_3MonthsAgo.name = tr( "3 Months Ago" );
    m_3MonthsAgo.timespan =
        TimeSpan( m_thisMonth.timespan.first.addMonths( -3 ),
                  m_thisMonth.timespan.second.addMonths( -3 ) );
    m_3MonthsAgo.timeSpanType = Month;

    m_thisYear.name = tr( "This year" );
    m_thisYear.timespan =
       TimeSpan( QDate( today.year(), 1, 1 ),
                 QDate( today.year(), 12, 31 ) );
    m_thisYear.timeSpanType = Year;
}

QList<NamedTimeSpan> TimeSpans::standardTimeSpans() const
{
    QList<NamedTimeSpan> spans;
    spans << m_today << m_yesterday << m_dayBeforeYesterday
          << m_thisWeek << m_lastWeek << m_theWeekBeforeLast
          << m_thisMonth << m_lastMonth << m_thisYear;
    return spans;
}

QList<NamedTimeSpan> TimeSpans::last4Weeks() const
{
    QList<NamedTimeSpan> spans;
    spans << m_thisWeek << m_lastWeek << m_theWeekBeforeLast << m_3WeeksAgo;
    return spans;
}

QList<NamedTimeSpan> TimeSpans::last4Months() const
{
    QList<NamedTimeSpan> spans;
    spans << m_thisMonth << m_lastMonth << m_theMonthBeforeLast << m_3MonthsAgo;
    return spans;
}

NamedTimeSpan TimeSpans::today() const
{
    return m_today;
}

NamedTimeSpan TimeSpans::yesterday() const
{
    return m_yesterday;
}

NamedTimeSpan TimeSpans::dayBeforeYesterday() const
{
    return m_dayBeforeYesterday;
}

NamedTimeSpan TimeSpans::thisWeek() const
{
    return m_thisWeek;
}

NamedTimeSpan TimeSpans::lastWeek() const
{
    return m_lastWeek;
}

NamedTimeSpan TimeSpans::theWeekBeforeLast() const
{
    return m_theWeekBeforeLast;
}

NamedTimeSpan TimeSpans::thisMonth() const
{
    return m_thisMonth;
}

NamedTimeSpan TimeSpans::lastMonth() const
{
    return m_lastMonth;
}

NamedTimeSpan TimeSpans::theMonthBeforeLast() const
{
    return m_theMonthBeforeLast;
}

NamedTimeSpan TimeSpans::thisYear() const
{
    return m_thisYear;
}

bool NamedTimeSpan::contains( const QDate& date ) const
{
    return date >= timespan.first && date < timespan.second;
}

DateChangeWatcher::DateChangeWatcher( QObject* parent )
    : QObject( parent )
{
    connect( &m_timer, SIGNAL(timeout()), SLOT(slotTimeout()) );
    m_timer.start( 1000 * 60 );
    slotTimeout();
}

void DateChangeWatcher::slotTimeout()
{
    const QDate today = QDate::currentDate();
    if ( m_today == today )
        return;

    m_today = today;
    emit dateChanged();
}

#include "moc_TimeSpans.cpp"
