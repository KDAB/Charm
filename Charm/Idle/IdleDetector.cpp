#include <QtAlgorithms>

#include "Core/Configuration.h"

#include "IdleDetector.h"
#include "X11IdleDetector.h"

// FIXME verify inclusion guard is needed (Q_OBJECT?)
#if defined Q_WS_MAC
#include "MacOsIdleDetector.h"
#endif

IdleDetector::IdleDetector( QObject* parent )
    : QObject( parent )
{
}

IdleDetector* IdleDetector::createIdleDetector( QObject* parent )
{
#if defined Q_WS_MAC
#if defined QT_MAC_USE_COCOA
    // FIXME port Carbon Idle Detector to Cocoa
    return 0;
#else
    return new MacOsIdleDetector( parent );
#endif
#elif defined Q_WS_WIN
    // FIXME implement Windows Idle Detector
    // return new ...
    return 0;
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

void IdleDetector::maybeIdle( IdlePeriod period )
{
    if ( ! Configuration::instance().detectIdling ) {
        return;
    }

#ifdef NDEBUG
    const int MinimumSeconds = 180;
#else
    const int MinimumSeconds = 10;
#endif
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
        if ( first.first.secsTo( first.second ) >= MinimumSeconds ) {
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
