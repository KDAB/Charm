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
    Q_ASSERT( wdweek == 1 );
    Q_ASSERT( wdyear == year );
    //now go to the requested weekday, in week 1:
    start = start.addDays( day - start.dayOfWeek() );
    //now go forward to the requested week no.
    const QDate date = start.addDays( 7 * ( week - 1 ) );
    return date;
}
