/*
  Dates.cpp

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

#include "Dates.h"

#include <QDate>

//find date for a certain weekday in week no/year
QDate Charm::dateByWeekNumberAndWeekDay( int year, int week, int day ) {
    QDate start( year, 1, 1 );
    if ( start.weekNumber() != 1 ) // if Jan 1st is not in the week 1 (but week 53 of the previous year), add a week
        start = start.addDays( 7 );
    int wdyear = 0;
    const int wdweek = start.weekNumber( &wdyear );
    // now we really should be in week 1 of year
    Q_ASSERT( wdweek == 1 ); Q_UNUSED( wdweek );
    Q_ASSERT( wdyear == year );
    //now go to the requested weekday, in week 1:
    start = start.addDays( day - start.dayOfWeek() );
    //now go forward to the requested week no.
    const QDate date = start.addDays( 7 * ( week - 1 ) );
    return date;
}

QDate Charm::weekDayInWeekOf( Qt::DayOfWeek dayOfWeek, const QDate& date ) {
    return date.addDays( dayOfWeek - date.dayOfWeek() );
}

int Charm::numberOfWeeksInYear( int year )
{
    QDate d(year, 1, 1);
    d = d.addDays(d.daysInYear() - 1);
    int weeksInYear = d.weekNumber() == 1 ? 52 : d.weekNumber();
    Q_ASSERT(weeksInYear == 52 || weeksInYear == 53);
    return weeksInYear;
}

int Charm::weekDifference( const QDate &from, const QDate &to )
{
  int fromWeekYear, toWeekYear;
  const int fromWeekNumber = from.weekNumber(&fromWeekYear);
  const int toWeekNumber = to.weekNumber(&toWeekYear);
  int weeksForInterveningYears = 0;
  for (int year = fromWeekYear; year < toWeekYear; ++year)
    weeksForInterveningYears += numberOfWeeksInYear(year);

  return toWeekNumber + weeksForInterveningYears - fromWeekNumber;
}
