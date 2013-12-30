#ifndef DATESTESTS_H
#define DATESTESTS_H

#include <QObject>

class DatesTests : public QObject
{
    Q_OBJECT

private slots:
    void testDateByWeekNumberAndWorkDay();
    void testWeekDayInWeekOf();
    void testNumberOfWeeksInYear_data();
    void testNumberOfWeeksInYear();
    void testWeekDifference_data();
    void testWeekDifference();
};

#endif
