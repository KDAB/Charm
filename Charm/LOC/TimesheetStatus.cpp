#include "TimesheetStatus.h"

#include <QDate>
#include <QDebug>
#include <QXmlStreamReader>

TimesheetStatus::TimesheetStatus( QObject *parent )
    : QObject(parent)
    , m_missingYear(0)
    , m_missingWeek(0)
    , m_missingTimesheet(false)
{
}

TimesheetStatus::~TimesheetStatus()
{
}

bool TimesheetStatus::parse( const QByteArray &payload )
{
    const QDate currentDate = QDate::currentDate();
    const QDate lastWeekDate = currentDate.addDays(-currentDate.dayOfWeek());
    const int lastWeekYear = lastWeekDate.year();
    const int lastWeekNumber = lastWeekDate.weekNumber();

    // reset missing timesheet status
    m_missingWeek = 0;
    m_missingYear = 0;
    m_missingTimesheet = false;

    QXmlStreamReader xml( payload );
    while (!xml.atEnd()) {
        switch (xml.readNext()) {
        case QXmlStreamReader::StartElement: {
            const QStringRef token = xml.name();
            if (token.compare("timesheet") == 0) {
                const int year =
                    xml.attributes().value(QLatin1String("year")).toString().toInt();
                const int week =
                    xml.attributes().value(QLatin1String("week")).toString().toInt();

                /*
                 * How Week Comparison Works
                 *
                 * Year is offset by two positions allowing us to build a
                 * unique week identifier by mixing both the year and week
                 * number.
                 *
                 *  E.g. Year 2014 Week 01 -> 201401 (LAST WEEK)
                 *       Year 2013 Week 52 -> 201301 (WEEK BEFORE LAST & LAST TIMESHEET)
                 *
                 *       We then check if 201301 is lower than 201401, if so,
                 *       Bill nags Peter.
                 */
                if ((year * 100) + week < (lastWeekYear * 100) + lastWeekNumber) {
                    m_missingWeek = lastWeekNumber;
                    m_missingYear = lastWeekYear;
                    m_missingTimesheet = true;
                    return true;
                }
            }
            } break;

        default: continue;
        }
    }

    if (xml.hasError()) {
        qWarning() << "Failed to correctly parse timesheet status payload: " << xml.errorString();
        return false;
    }

    return true;
}

bool TimesheetStatus::isMissingTimesheet() const
{
    return m_missingTimesheet;
}

int TimesheetStatus::missingTimesheetYear() const
{
    return m_missingYear;
}

int TimesheetStatus::missingTimesheetWeek() const
{
    return m_missingWeek;
}

