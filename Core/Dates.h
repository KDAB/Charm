/*
  Dates.h

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

#ifndef CHARM_DATES_H
#define CHARM_DATES_H

#include <QDate>

namespace Charm {
    QDate dateByWeekNumberAndWeekDay( int year, int week, int weekday );
    /**
     * returns the date for a week day @p dayOfWeek, in the week of date @p date.
     */
    QDate weekDayInWeekOf( Qt::DayOfWeek dayOfWeek, const QDate& date );

    int numberOfWeeksInYear( int year );

    int weekDifference( const QDate &from, const QDate &to );
}

#endif // CHARM_DATES_H
