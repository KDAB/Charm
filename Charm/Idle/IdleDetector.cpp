#include <QtAlgorithms>
#include <QDebug>

#include "Core/Configuration.h"
#include "CharmCMake.h"

#include <QScopedPointer>

#include "IdleDetector.h"
#include "MacIdleDetector.h"
#include "WindowsIdleDetector.h"
#include "X11IdleDetector.h"

IdleDetector::IdleDetector( QObject* parent )
    : QObject( parent )
    , m_idlenessDuration( CHARM_IDLE_TIME ) // from CharmCMake.h
    , m_available( true )
{
}

IdleDetector* IdleDetector::createIdleDetector( QObject* parent )
{
#ifdef CHARM_IDLE_DETECTION
#ifdef Q_OS_OSX
    return new MacIdleDetector( parent );
#endif

#ifdef Q_OS_WIN
    return new WindowsIdleDetector( parent );
#endif

#ifdef CHARM_IDLE_DETECTION_AVAILABLE_X11
    X11IdleDetector* detector = new X11IdleDetector( parent );
    detector->setAvailable( X11IdleDetector::idleCheckPossible() );
    return detector;
#endif
#endif

    IdleDetector* unavailable = new IdleDetector( parent );
    unavailable->setAvailable( false );
    return unavailable;
}

bool IdleDetector::available() const
{
    return m_available;
}

void IdleDetector::setAvailable( bool available )
{
    if ( m_available == available )
        return;
    m_available = available;
    emit availableChanged( m_available );
}

IdleDetector::IdlePeriods IdleDetector::idlePeriods() const
{
    return m_idlePeriods;
}

void IdleDetector::setIdlenessDuration( int seconds ) {
    if ( m_idlenessDuration == seconds )
        return;
    m_idlenessDuration = seconds;
    emit idlenessDurationChanged( m_idlenessDuration );
    onIdlenessDurationChanged();
}

int IdleDetector::idlenessDuration() const {
    return m_idlenessDuration;
}

void IdleDetector::maybeIdle( IdlePeriod period )
{
    if ( ! Configuration::instance().detectIdling ) {
        return;
    }

    qDebug() << "IdleDetector::maybeIdle: Checking for idleness";

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
        qDebug() << "IdleDetector::maybeIdle: Found idleness";
        emit maybeIdle();
    }
}

void IdleDetector::clear()
{
    m_idlePeriods.clear();
}

#include "moc_IdleDetector.cpp"
