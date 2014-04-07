#ifndef MONTHLYTIMESHEETXMLWRITER_H
#define MONTHLYTIMESHEETXMLWRITER_H

#include "Core/Event.h"
#include "Core/Task.h"

class QByteArray;
class CharmDataModel;

class MonthlyTimesheetXmlWriter {
public:
    MonthlyTimesheetXmlWriter();
    /**
     * @throws XmlSerializationException
     */
    QByteArray saveToXml() const;

    void setDataModel( const CharmDataModel* dataModel );
    void setYearOfMonth( int yearOfMonth );
    void setMonthNumber( int monthNumber );
    void setNumberOfWeeks( int numberOfWeeks );
    void setEvents( const EventList& events );
    void setRootTask( TaskId rootTask );

private:
    const CharmDataModel* m_dataModel;
    int m_yearOfMonth;
    int m_monthNumber;
    int m_numberOfWeeks;
    TaskId m_rootTask;
    EventList m_events;
};

#endif
