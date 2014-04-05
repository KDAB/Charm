#ifndef TIMESHEETSTATUSTESTS_H
#define TIMESHEETSTATUSTESTS_H

#include "Charm/LOC/TimesheetStatus.h"

class TimesheetStatusTests : public TimesheetStatus
{
    Q_OBJECT

public:
    explicit TimesheetStatusTests( QObject *parent = 0 );

private slots:
    void testParseTimesheetStatus();
};

#endif
