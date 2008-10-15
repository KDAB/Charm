#include "X11IdleDetector.h"
#include <QX11Info>
#include <QTimer>
#include "config-charm.h"

#if defined(Q_WS_X11) && defined(HAVE_LIBXSS)
 #include <X11/Xlib.h>
 #include <X11/Xutil.h>
 #include <X11/extensions/scrnsaver.h>
#endif // HAVE_LIBXSS

const int IDLE_CHECK_INTERVAL= 30; // In seconds
const int PERIOD_FOR_IDLENESS = 10*60; // In seconds

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
    QTimer* timer = new QTimer( this );
    connect( timer, SIGNAL( timeout() ), this, SLOT( checkIdleness() ) );
    timer->start( IDLE_CHECK_INTERVAL * 1000 );
}

void X11IdleDetector::checkIdleness()
{
#if defined(Q_WS_X11) && defined(HAVE_LIBXSS)
    XScreenSaverInfo* _mit_info = XScreenSaverAllocInfo();
    XScreenSaverQueryInfo(QX11Info::display(), QX11Info::appRootWindow(), _mit_info);
    int idleSecs = _mit_info->idle/1000;

    if (idleSecs >= PERIOD_FOR_IDLENESS)
        maybeIdle( qMakePair(QDateTime::currentDateTime().addSecs( -idleSecs ), QDateTime::currentDateTime() ) );
#endif // HAVE_LIBXSS

}

#include "X11IdleDetector.moc"
