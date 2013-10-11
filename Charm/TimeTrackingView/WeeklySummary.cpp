#include <QFontMetrics>

#include "WeeklySummary.h"

static const int DAYS_IN_WEEK = 7;

WeeklySummary::WeeklySummary()
    : task( 0 ), durations( DAYS_IN_WEEK, 0 ) {
}
