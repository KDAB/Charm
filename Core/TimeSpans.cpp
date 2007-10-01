#include <QtDebug>
#include <QTimer>
#include <QTime>

#include "TimeSpans.h"

bool NamedTimeSpan::contains( const QDate& date )
{
    return date >= timespan.first && date < timespan.second;
}

TimeSpans::TimeSpans( QObject* parent )
    : QObject( parent )
{
    slotUpdateTimeSpans();

    connect( &m_timer, SIGNAL( timeout() ), SLOT( slotUpdateTimeSpans() ) );
    m_timer.start( 1000* 60 );
}

const NamedTimeSpan& TimeSpans::today() const
{
    return m_today;
}

const NamedTimeSpan& TimeSpans::yesterday() const
{
    return m_yesterday;
}

const NamedTimeSpan& TimeSpans::dayBeforeYesterday() const
{
    return m_dayBeforeYesterday;
}

const NamedTimeSpan& TimeSpans::thisWeek() const
{
    return m_thisWeek;
}

const NamedTimeSpan& TimeSpans::lastWeek() const
{
    return m_lastWeek;
}

const NamedTimeSpan& TimeSpans::theWeekBeforeLast() const
{
    return m_theWeekBeforeLast;
}

const NamedTimeSpan& TimeSpans::thisMonth() const
{
    return m_thisMonth;
}

const NamedTimeSpan& TimeSpans::lastMonth() const
{
    return m_lastMonth;
}

QList<NamedTimeSpan> TimeSpans::standardTimeSpans()
{
    QList<NamedTimeSpan> spans;
    spans << m_today << m_yesterday << m_dayBeforeYesterday
          << m_thisWeek << m_lastWeek << m_theWeekBeforeLast
          << m_thisMonth << m_lastMonth;
    return spans;
}

QList<NamedTimeSpan> TimeSpans::last4Weeks()
{
    QList<NamedTimeSpan> spans;
    spans << m_thisWeek << m_lastWeek << m_theWeekBeforeLast << m_3WeeksAgo;
    return spans;
}

void TimeSpans::slotUpdateTimeSpans()
{   // if today is unchanged, return
    if ( m_today.timespan.first == QDate::currentDate() )
        return;

    // if we are just starting up, continue
    if ( ! m_today.timespan.first.isValid() ) {
//         qDebug() << "TimeSpans::slotUpdateTimeSpans: today is"
//                  << "today. The world is round.";
    } else { // today has passed (for example, the computer was suspended)
//         qDebug() << "TimeSpans::slotUpdateTimeSpans: today is yesterday."
//                  << "Slavery is freedom. The world is flat.";
    }

    QTime now = QTime::currentTime();
//     qDebug() << "TimeSpans::slotUpdateTimeSpans: it is now"
//              << now << ", updating timespans.";

    QDate today = QDate::currentDate();

    m_today.name = tr( "Today" );
    m_today.timespan =
        TimeSpan( today,
                  today.addDays( 1 ) );

    m_yesterday.name = tr( "Yesterday" );
    m_yesterday.timespan =
        TimeSpan( m_today.timespan.first.addDays( -1 ),
                  m_today.timespan.second.addDays( -1 ) );

    m_dayBeforeYesterday.name = tr( "The Day Before Yesterday" );
    m_dayBeforeYesterday.timespan =
        TimeSpan( m_today.timespan.first.addDays( -2 ),
                  m_today.timespan.second.addDays( -2 ) );

    m_thisWeek.name = tr( "This Week" );
    m_thisWeek.timespan =
        TimeSpan( today.addDays( - today.dayOfWeek() + 1 ),
                  today.addDays( 7 - today.dayOfWeek() + 1 ) );

    m_lastWeek.name = tr( "Last Week" );
    m_lastWeek.timespan =
        TimeSpan( m_thisWeek.timespan.first.addDays( -7 ),
                  m_thisWeek.timespan.second.addDays( -7 ) );

    m_theWeekBeforeLast.name = tr( "The Week Before Last Week" );
    m_theWeekBeforeLast.timespan =
        TimeSpan( m_thisWeek.timespan.first.addDays( -14 ),
                  m_thisWeek.timespan.second.addDays( -14 ) );

    m_3WeeksAgo.name = tr( "3 Weeks Ago" );
    m_3WeeksAgo.timespan =
        TimeSpan( m_thisWeek.timespan.first.addDays( -21 ),
                  m_thisWeek.timespan.second.addDays( -21 ) );

    m_thisMonth.name = tr( "This Month" );
    m_thisMonth.timespan =
        TimeSpan( today.addDays( - today.day() + 1 ),
                  today.addDays( today.daysInMonth() - today.day() + 1) );

    m_lastMonth.name = tr( "Last Month" );
    m_lastMonth.timespan =
        TimeSpan( m_thisMonth.timespan.first.addMonths( -1 ),
                  m_thisMonth.timespan.second.addMonths( -1 ) );

    emit timeSpansChanged();
//     qDebug() << "TimeSpans::slotUpdateTimeSpans: this is where we ar at:" << endl
//              << "--> today:" << m_today.timespan << endl
//              << "--> yesterday:" << m_yesterday.timespan << endl
//              << "--> day before yesterday:" << m_dayBeforeYesterday.timespan << endl
//              << "--> this week:" << m_thisWeek.timespan << endl
//              << "--> last week:" << m_lastWeek.timespan << endl
//              << "--> week before last:" << m_theWeekBeforeLast.timespan << endl
//              << "--> this month:" << m_thisMonth.timespan << endl
//              << "--> last month:" << m_lastMonth.timespan << endl
//              << "--> next update in" << msecsUntil5SecsPastMidnight / 60000 << "minutes.";
}

#include "TimeSpans.moc"
