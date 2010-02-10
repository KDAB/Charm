#include "MacCocoaIdleDetector.h"

#include <QDateTime>

@interface MacCocoaIdleObserver : NSObject
{
@public
    MacCocoaIdleDetector* cocoaIdleDetector;
    QDateTime idleStartTime;
}
- (id)init;
- (void)dealloc;
@end

@implementation MacCocoaIdleObserver
- (id)init {
    if ((self = [super init])) {
        NSNotificationCenter* notificationCenter = [[NSWorkspace sharedWorkspace] notificationCenter];
        [notificationCenter
         addObserver: self
         selector: @selector(receiveSleepNotification:)
         name: NSWorkspaceWillSleepNotification
         object: NULL ];
        [notificationCenter
         addObserver: self
         selector: @selector(receiveWakeNotification:)
         name: NSWorkspaceDidWakeNotification
         object: NULL ];
        [notificationCenter
         addObserver: self
         selector: @selector(receiveSleepNotification:)
         name: NSWorkspaceScreensDidSleepNotification
         object: NULL ];
        [notificationCenter
         addObserver: self
         selector: @selector(receiveWakeNotification:)
         name: NSWorkspaceScreensDidWakeNotification
         object: NULL ];
    }
    return self;
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [super dealloc];
}

- (void)receiveSleepNotification:(NSNotification*)notification {
    idleStartTime = QDateTime::currentDateTime();
}

- (void)receiveWakeNotification:(NSNotification*)notification {
    if (cocoaIdleDetector)
        cocoaIdleDetector->idle();
}
@end

MacCocoaIdleDetector::MacCocoaIdleDetector( QObject* parent )
    : IdleDetector( parent )
    , m_observer( [[MacCocoaIdleObserver alloc] init] )
{
    MacCocoaIdleObserver* observer =
            static_cast<MacCocoaIdleObserver*>(m_observer);
    observer->cocoaIdleDetector = this;
}

void MacCocoaIdleDetector::idle()
{
    MacCocoaIdleObserver* observer = static_cast<MacCocoaIdleObserver*>(m_observer);
    emit maybeIdle( IdlePeriod( observer->idleStartTime, QDateTime::currentDateTime() ) );
}

#include "MacCocoaIdleDetector.moc"
