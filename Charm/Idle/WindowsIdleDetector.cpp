#include "WindowsIdleDetector.h"

#include "windows.h"

#include <QDebug>

WindowsIdleDetector::WindowsIdleDetector( QObject* parent ) : IdleDetector( parent ) {
    connect( &m_timer, SIGNAL(timeout()), this, SLOT(timeout()) );
    m_timer.setInterval( idlenessDuration() * 1000 / 2 );
    m_timer.setSingleShot( false );
    m_timer.start();
}

void WindowsIdleDetector::onIdlenessDurationChanged() {
    m_timer.stop();
    m_timer.setInterval( idlenessDuration() * 1000 / 2 );
    m_timer.start();
}

void WindowsIdleDetector::timeout() {
    LASTINPUTINFO lif;
    lif.cbSize = sizeof( lif );
    const bool ret = GetLastInputInfo( &lif );
    if ( !ret ) {
        qWarning() << "Idle detection: GetLastInputInfo failed.";
        return;
    }

    const qint64 dwTime = static_cast<qint64>( lif.dwTime );
    const qint64 ctk = static_cast<qint64>( GetTickCount() );
    const int idleSecs = ( ctk - dwTime ) / 1000;
    if ( idleSecs >= idlenessDuration() )
        maybeIdle( IdlePeriod(QDateTime::currentDateTime().addSecs( -idleSecs ),
                              QDateTime::currentDateTime() ) );
}

#include "moc_WindowsIdleDetector.cpp"
