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
}

#endif // CHARM_DATES_H
