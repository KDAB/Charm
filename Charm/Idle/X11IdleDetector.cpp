#include "X11IdleDetector.h"
#include <QTimer>
#include "config-charm.h"

#if defined(Q_WS_X11) && defined(HAVE_LIBXSS)
 #include <QX11Info>
 #include <X11/Xlib.h>
 #include <X11/Xutil.h>
 #include <X11/extensions/scrnsaver.h>
#endif // HAVE_LIBXSS


bool X11IdleDetector::idleCheckPossible()
{
#if defined(Q_WS_X11) && defined(HAVE_LIBXSS)
    int event_base, error_base;
    if(XScreenSaverQueryExtension(QX11Info::display(), &event_base, &error_base))
        return true;
#endif
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
#if defined(Q_WS_X11) && defined(HAVE_LIBXSS)
    XScreenSaverInfo* _mit_info = XScreenSaverAllocInfo();
    XScreenSaverQueryInfo(QX11Info::display(), QX11Info::appRootWindow(), _mit_info);
    const int idleSecs = _mit_info->idle / 1000;

    if (idleSecs >= idlenessDuration())
        maybeIdle( IdlePeriod(QDateTime::currentDateTime().addSecs( -idleSecs ),
                              QDateTime::currentDateTime() ) );
#endif // HAVE_LIBXSS

    if ( m_heartbeat.secsTo( QDateTime::currentDateTime() ) > idlenessDuration() )
        maybeIdle( IdlePeriod( m_heartbeat, QDateTime::currentDateTime() ) );
    m_heartbeat = QDateTime::currentDateTime();
}

#include "X11IdleDetector.moc"
