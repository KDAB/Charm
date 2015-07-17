/*
  TimeSpans.h

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

#ifndef TIMESPANS_H
#define TIMESPANS_H

#include <QCoreApplication>
#include <QDate>
#include <QPair>
#include <QTimer>
#include <QObject>
#include <QDateTime>

typedef QPair<QDate, QDate> TimeSpan;

enum TimeSpanType {
    Day=0,
    Week,
    Month,
    Range,
    Year
};

struct NamedTimeSpan {
    QString name;
    TimeSpan timespan;
    bool contains( const QDate& date ) const;
    TimeSpanType timeSpanType;
};

/** Provides commonly used time spans for a given date.
    The spans are measured from a start time to *before* an end time.
    today() looks like this: today, 0:00 - tomorrow, 0:00
    To see of a date (or datetime) is within the span, test for
    x >= start && x < end.
    TimeSpan only deals with days, not with anything of finer
    granularity.
*/
class TimeSpans {
    Q_DECLARE_TR_FUNCTIONS(TimeSpans)
public:

    /**
     * Creates a collection of timespans with @p referenceDate as reference date.
     *
     * @param referenceDate the reference date ("today") to calculate time spans for
     */
    explicit TimeSpans( const QDate& referenceDate=QDate::currentDate() );

    QList<NamedTimeSpan> standardTimeSpans() const;
    QList<NamedTimeSpan> last4Weeks() const;
    QList<NamedTimeSpan> last4Months() const;

    NamedTimeSpan today() const;
    NamedTimeSpan yesterday() const;
    NamedTimeSpan dayBeforeYesterday() const;
    NamedTimeSpan thisWeek() const;
    NamedTimeSpan lastWeek() const;
    NamedTimeSpan theWeekBeforeLast() const;
    NamedTimeSpan thisMonth() const;
    NamedTimeSpan thisYear() const;
    NamedTimeSpan lastMonth() const;
    NamedTimeSpan theMonthBeforeLast() const;

private:
    NamedTimeSpan m_today;
    NamedTimeSpan m_yesterday;
    NamedTimeSpan m_dayBeforeYesterday;
    NamedTimeSpan m_thisWeek;
    NamedTimeSpan m_lastWeek;
    NamedTimeSpan m_theWeekBeforeLast;
    NamedTimeSpan m_3WeeksAgo;
    NamedTimeSpan m_thisMonth;
    NamedTimeSpan m_thisYear;
    NamedTimeSpan m_lastMonth;
    NamedTimeSpan m_theMonthBeforeLast;
    NamedTimeSpan m_3MonthsAgo;
};

class DateChangeWatcher : public QObject
{
    Q_OBJECT

public:
    explicit DateChangeWatcher( QObject* parent = nullptr );

signals:
    void dateChanged();

private slots:
    void slotTimeout();

private:
    QTimer m_timer;
    QDate m_today;
};

#endif
