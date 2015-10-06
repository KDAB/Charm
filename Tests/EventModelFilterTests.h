/*
  EventModelFilterTests.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2012-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

#ifndef EVENTMODELFILTERTESTS
#define EVENTMODELFILTERTESTS

#include <QObject>

#include "Core/TimeSpans.h"

class CharmDataModel;
class EventModelFilter;

class EventModelFilterTests : public QObject
{
    Q_OBJECT

public:
    EventModelFilterTests();

private slots:
    void initTestCase();
    void checkYearsFilter();
    void checkMonthsFilter();
    void checkWeeksFilter();
    void checkDaysFilter();
    void checkEventSpanOver2Weeks();
    void checkEventSpanOver2Days();

private:
    CharmDataModel* m_referenceModel;
    EventModelFilter* m_eventModelFilter;

    NamedTimeSpan m_thisYearSpan;
    NamedTimeSpan m_theMonthBeforeLastSpan;
    NamedTimeSpan m_lastMonthSpan;
    NamedTimeSpan m_thisMonthSpan;
    NamedTimeSpan m_theWeekBeforeLastSpan;
    NamedTimeSpan m_lastWeekSpan;
    NamedTimeSpan m_thisWeekSpan;
    NamedTimeSpan m_dayBeforeYesterdaySpan;
    NamedTimeSpan m_yesterdaySpan;
    NamedTimeSpan m_todaySpan;
    NamedTimeSpan m_everSpan;
};

#endif // EVENTMODELFILTERTESTS

