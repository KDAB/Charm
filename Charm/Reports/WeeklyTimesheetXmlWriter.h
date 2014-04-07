#ifndef WEEKLYTIMESHEETXMLWRITER_H
#define WEEKLYTIMESHEETXMLWRITER_H

#include "Core/Event.h"
#include "Core/Task.h"

class QByteArray;
class CharmDataModel;

class WeeklyTimesheetXmlWriter {
public:
    WeeklyTimesheetXmlWriter();
    /**
     * @throws XmlSerializationException
     */
    QByteArray saveToXml() const;

    void setDataModel( const CharmDataModel* model);
    void setYear( int year );
    void setWeekNumber( int weekNumber );
    void setEvents( const EventList& events );
    void setRootTask( TaskId rootTask );
private:
    const CharmDataModel* m_dataModel;
    int m_year;
    int m_weekNumber;
    TaskId m_rootTask;
    EventList m_events;
};

#endif
