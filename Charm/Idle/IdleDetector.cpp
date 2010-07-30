#include <QtAlgorithms>

#include "Core/Configuration.h"

#include "IdleDetector.h"
#include "X11IdleDetector.h"

// FIXME verify inclusion guard is needed (Q_OBJECT?)
#if defined Q_WS_MAC
#ifdef QT_MAC_USE_COCOA
#include "MacCocoaIdleDetector.h"
#else
#include "MacCarbonIdleDetector.h"
#endif
#endif

#ifdef Q_OS_WIN
# include "WindowsIdleDetector.h"
#endif

IdleDetector::IdleDetector( QObject* parent )
    : QObject( parent )
#ifdef NDEBUG
    , m_idlenessDuration( 10 )
#else
    , m_idlenessDuration( 3 * 60 )
#endif
{
}

IdleDetector* IdleDetector::createIdleDetector( QObject* parent )
{
#if defined Q_WS_MAC
#if defined QT_MAC_USE_COCOA
    return new MacCocoaIdleDetector( parent );
#else
    return new MacCarbonIdleDetector( parent );
#endif
#elif defined Q_WS_WIN
    return new WindowsIdleDetector( parent );
#elif defined Q_WS_X11
    if ( X11IdleDetector::idleCheckPossible() ) {
        return new X11IdleDetector( parent );
    } else {
        return 0;
    }
#else
    return 0;
#endif
}

IdleDetector::IdlePeriods IdleDetector::idlePeriods() const
{
    return m_idlePeriods;
}

void IdleDetector::setIdlenessDuration( int seconds ) {
    if ( m_idlenessDuration == seconds )
        return;
    m_idlenessDuration = seconds;
    idlenessDurationChanged();
}

int IdleDetector::idlenessDuration() const {
    return m_idlenessDuration;
}

void IdleDetector::maybeIdle( IdlePeriod period )
{
    if ( ! Configuration::instance().detectIdling ) {
        return;
    }

    // merge overlapping idle periods
    IdlePeriods periods ( idlePeriods() );
    periods << period;
//     // TEMP (this was used to test the overlapping-idle-period compression below, leave it in
//     {
//         IdlePeriod i2( period.first.addSecs( 1 ), period.second.addSecs( 1 ) ); // should be merged
//         IdlePeriod i3( period.second.addSecs( 2 ), period.second.addSecs( 5 ) ); // should not be merged
//         periods << i2 << i3;
//     }

    qSort( periods );
    m_idlePeriods.clear();
    while ( ! periods.isEmpty() ) {
        IdlePeriod first = periods.first();
        periods.pop_front();
        while ( ! periods.isEmpty() ) {
            IdlePeriod second = periods.first();
            if ( second.first >= first.first && second.first <= first.second ) {
                first.second = qMax( first.second, second.second );
                // first.first is already the earlier time, because the container is sorted
            } else {
                break;
            }
            periods.pop_front();
        }
        if ( first.first.secsTo( first.second ) >= idlenessDuration() ) {
            // we ignore idle period of less than MinimumSeconds
            m_idlePeriods << first;
        }
    }
    // notify application
    if ( ! idlePeriods().isEmpty() ) {
        emit maybeIdle();
    }
}

void IdleDetector::clear()
{
    m_idlePeriods.clear();
}

#include "IdleDetector.moc"
