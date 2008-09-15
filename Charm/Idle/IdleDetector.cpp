#include "IdleDetector.h"

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
    return new MacOsIdleDetector( parent );
#elif defined Q_WS_WIN
    // FIXME implement Windows Idle Detector
    // return new ...
#elif defined Q_WS_X11
    // FIXME implement X11 Idle Detector
    // return new ...
#else
    return 0;
#endif
}

#include "IdleDetector.moc"
