#include "MacCocoaApplication.h"

#include <QShortcutEvent>

MacCocoaApplication::MacCocoaApplication( int& argc, char* argv[] )
    : MacApplication( argc, argv )
{
    [[NSAutoreleasePool alloc] init];

    // TODO: Handle kEventClassAppleEvent/kAEReopenApplication somehow for
    // dockIconClicked. Use applicationShouldHandleReopen delegate or
    // legacy Apple Event handling.
    m_eventMonitor = [NSEvent addLocalMonitorForEventsMatchingMask:
                       NSKeyDownMask handler:^(NSEvent *incomingEvent) {
        return cocoaEventFilter(incomingEvent);
    }];
}

MacCocoaApplication::~MacCocoaApplication()
{
    [NSEvent removeMonitor:m_eventMonitor];
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
