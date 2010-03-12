#include <Cocoa/Cocoa.h>

#include "MacCocoaApplication.h"

#include <QShortcutEvent>

@interface DockIconClickEventHandler : NSObject
{
@public
    MacCocoaApplication* macCocoaApplication;
}
- (void)handleDockClickEvent:(NSAppleEventDescriptor*)event withReplyEvent:(NSAppleEventDescriptor*)replyEvent;
@end

@implementation DockIconClickEventHandler
- (void)handleDockClickEvent:(NSAppleEventDescriptor*)event withReplyEvent:(NSAppleEventDescriptor*)replyEvent {
    if (macCocoaApplication)
        macCocoaApplication->dockIconClickEvent();
}
@end

MacCocoaApplication::MacCocoaApplication( int& argc, char* argv[] )
    : MacApplication( argc, argv ),
    m_dockIconClickEventHandler([[DockIconClickEventHandler alloc] init])
{
    [[NSAutoreleasePool alloc] init];

    m_eventMonitor = [NSEvent
                      addLocalMonitorForEventsMatchingMask:NSKeyDownMask
                      handler:^(NSEvent *incomingEvent) {
        return cocoaEventFilter(incomingEvent);
    }];

    DockIconClickEventHandler* dockIconClickEventHandler =
            static_cast<DockIconClickEventHandler*>(m_dockIconClickEventHandler);
    dockIconClickEventHandler->macCocoaApplication = this;
}

MacCocoaApplication::~MacCocoaApplication()
{
    [NSEvent removeMonitor:m_eventMonitor];
}

void MacCocoaApplication::setupCocoaEventHandler() const
{
    // TODO: This apparently uses a legacy API and we should be using the
    // applicationShouldHandleReopen:hasVisibleWindows: method on
    // NSApplicationDelegate but this isn't possible without nasty runtime
    // reflection hacks until Qt is fixed. If this breaks, shout at them :)
    [[NSAppleEventManager sharedAppleEventManager]
     setEventHandler:m_dockIconClickEventHandler
     andSelector:@selector(handleDockClickEvent:withReplyEvent:)
     forEventClass:kCoreEventClass
     andEventID:kAEReopenApplication];
}

void MacCocoaApplication::dockIconClickEvent()
{
    emit dockIconClicked();
}

NSEvent* MacCocoaApplication::cocoaEventFilter( NSEvent* incomingEvent )
{
    NSUInteger modifierFlags = [incomingEvent modifierFlags];

    int shortcutFlags = [[incomingEvent charactersIgnoringModifiers] UTF8String][0];

    if (modifierFlags & NSShiftKeyMask)
        shortcutFlags |= Qt::ShiftModifier;
    if (modifierFlags & NSControlKeyMask)
        shortcutFlags |= Qt::MetaModifier;
    if (modifierFlags & NSCommandKeyMask)
        shortcutFlags |= Qt::ControlModifier;
    if (modifierFlags & NSAlternateKeyMask)
        shortcutFlags |= Qt::AltModifier;

    const QKeySequence keySequence( shortcutFlags );
    const bool autoRepeat = [incomingEvent isARepeat];

    const QList< QShortcut* > active = activeShortcuts( keySequence, autoRepeat );
    foreach( QShortcut* const shortcut, active )
    {
        QShortcutEvent event( keySequence, shortcut->id() );
        QObject* const receiver = shortcut;
        receiver->event( &event );
    }

    if (!active.isEmpty())
        return nil;

    return incomingEvent;
}

#include "MacCocoaApplication.moc"
