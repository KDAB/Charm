#include "X11IdleDetector.h"
#include <QTimer>
#include "config-charm.h"

#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/scrnsaver.h>

bool X11IdleDetector::idleCheckPossible()
{
    int event_base, error_base;
    if(XScreenSaverQueryExtension(QX11Info::display(), &event_base, &error_base))
        return true;
    return false;
}

X11IdleDetector::X11IdleDetector( QObject* parent )
    : IdleDetector( parent )
{
    connect( &m_timer, SIGNAL( timeout() ), this, SLOT( checkIdleness() ) );
    m_timer.start( idlenessDuration() * 1000 / 5 );
    m_heartbeat = QDateTime::currentDateTime();
}

void X11IdleDetector::idlenessDurationChanged()
{
    m_timer.stop();
    m_timer.start( idlenessDuration() * 1000 / 5 );
}

void X11IdleDetector::checkIdleness()
{
    XScreenSaverInfo* _mit_info = XScreenSaverAllocInfo();
    if (!_mit_info)
        return;
    XScreenSaverQueryInfo(QX11Info::display(), QX11Info::appRootWindow(), _mit_info);
    const int idleSecs = _mit_info->idle / 1000;
    XFree(_mit_info);

    if (idleSecs >= idlenessDuration())
        maybeIdle( IdlePeriod(QDateTime::currentDateTime().addSecs( -idleSecs ),
                              QDateTime::currentDateTime() ) );

    if ( m_heartbeat.secsTo( QDateTime::currentDateTime() ) > idlenessDuration() )
        maybeIdle( IdlePeriod( m_heartbeat, QDateTime::currentDateTime() ) );
    m_heartbeat = QDateTime::currentDateTime();
}

#include "X11IdleDetector.moc"
