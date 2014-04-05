#ifndef LOC_TIMESHEETSTATUS_H
#define LOC_TIMESHEETSTATUS_H

#include <QObject>

class TimesheetStatus : public QObject
{
    Q_OBJECT
public:

    explicit TimesheetStatus( QObject *parent = 0 );
    ~TimesheetStatus();

    bool parse( const QByteArray &payload );

    bool isMissingTimesheet() const;
    int missingTimesheetYear() const;
    int missingTimesheetWeek() const;

private:
    int m_missingYear;
    int m_missingWeek;
    bool m_missingTimesheet;
};

#endif //LOC_TIMESHEETSTATUS_H
