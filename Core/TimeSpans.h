#ifndef TIMESPANS_H
#define TIMESPANS_H

#include <QDate>
#include <QPair>
#include <QTimer>
#include <QObject>
#include <QDateTime>

typedef QPair<QDate, QDate> TimeSpan;

struct NamedTimeSpan {
    QString name;
    TimeSpan timespan;
    bool contains( const QDate& date );
};

/** TimeSpans provides up-to-date spans of time.
    The spans are measured from a start time to *before* an end time.
    today() looks like this: today, 0:00 - tomorrow, 0:00
    To see of a date (or datetime) is within the span, test for
    x >= start && x < end.
    TimeSpan only deals with days, not with anything of finer
    granularity.
*/
class TimeSpans : public QObject
{
    Q_OBJECT

public:
    explicit TimeSpans( QObject* parent = 0 );

    QList<NamedTimeSpan> standardTimeSpans();
    QList<NamedTimeSpan> last4Weeks();

    const NamedTimeSpan& today() const;
    const NamedTimeSpan& yesterday() const;
    const NamedTimeSpan& dayBeforeYesterday() const;
    const NamedTimeSpan& thisWeek() const;
    const NamedTimeSpan& lastWeek() const;
    const NamedTimeSpan& theWeekBeforeLast() const;
    const NamedTimeSpan& thisMonth() const;
    const NamedTimeSpan& lastMonth() const;

signals:
    void timeSpansChanged();

private slots:
    // timer triggered:
    void slotUpdateTimeSpans();

private:
    // QDate m_lastToday;
    NamedTimeSpan m_today;
    NamedTimeSpan m_yesterday;
    NamedTimeSpan m_dayBeforeYesterday;
    NamedTimeSpan m_thisWeek;
    NamedTimeSpan m_lastWeek;
    NamedTimeSpan m_theWeekBeforeLast;
    NamedTimeSpan m_3WeeksAgo;
    NamedTimeSpan m_thisMonth;
    NamedTimeSpan m_lastMonth;
    QTimer m_timer;
};

#endif
