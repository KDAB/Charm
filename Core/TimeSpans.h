#ifndef TIMESPANS_H
#define TIMESPANS_H

#include <QCoreApplication>
#include <QDate>
#include <QPair>
#include <QTimer>
#include <QObject>
#include <QDateTime>

typedef QPair<QDate, QDate> TimeSpan;

struct NamedTimeSpan {
    QString name;
    TimeSpan timespan;
    bool contains( const QDate& date ) const;
};

/** Provides commonly used time spans for a given date.
    The spans are measured from a start time to *before* an end time.
    today() looks like this: today, 0:00 - tomorrow, 0:00
    To see of a date (or datetime) is within the span, test for
    x >= start && x < end.
    TimeSpan only deals with days, not with anything of finer
    granularity.
*/
class TimeSpans {
    Q_DECLARE_TR_FUNCTIONS(TimeSpans)
public:

    /**
     * Creates a collection of timespans with @p referenceDate as reference date.
     *
     * @param referenceDate the reference date ("today") to calculate time spans for
     */
    explicit TimeSpans( const QDate& referenceDate=QDate::currentDate() );

    QList<NamedTimeSpan> standardTimeSpans() const;
    QList<NamedTimeSpan> last4Weeks() const;
    QList<NamedTimeSpan> last4Months() const;

    NamedTimeSpan today() const;
    NamedTimeSpan yesterday() const;
    NamedTimeSpan dayBeforeYesterday() const;
    NamedTimeSpan thisWeek() const;
    NamedTimeSpan lastWeek() const;
    NamedTimeSpan theWeekBeforeLast() const;
    NamedTimeSpan thisMonth() const;
    NamedTimeSpan lastMonth() const;
    NamedTimeSpan theMonthBeforeLast() const;

private:
    NamedTimeSpan m_today;
    NamedTimeSpan m_yesterday;
    NamedTimeSpan m_dayBeforeYesterday;
    NamedTimeSpan m_thisWeek;
    NamedTimeSpan m_lastWeek;
    NamedTimeSpan m_theWeekBeforeLast;
    NamedTimeSpan m_3WeeksAgo;
    NamedTimeSpan m_thisMonth;
    NamedTimeSpan m_lastMonth;
    NamedTimeSpan m_theMonthBeforeLast;
    NamedTimeSpan m_3MonthsAgo;
};

class DateChangeWatcher : public QObject
{
    Q_OBJECT

public:
    explicit DateChangeWatcher( QObject* parent = 0 );

signals:
    void dateChanged();

private slots:
    void slotTimeout();

private:
    QTimer m_timer;
    QDate m_today;
};

#endif
