#include <QDateTime>
#include <QTimer>
#include <QtDebug>

#if defined Q_WS_MAC

#include <mach/mach_port.h>
#include <mach/mach_interface.h>
#include <mach/mach_init.h>
#include <IOKit/pwr_mgt/IOPMLib.h>
#include <IOKit/IOMessage.h>
#include <Carbon/Carbon.h>

#include "MacOsIdleDetector.h"

class MacOsIdleDetector::Private
{
public:
    // a reference to the Root Power Domain IOService
    io_connect_t root_port;
    // notification port allocated by IORegisterForSystemPower
    IONotificationPortRef notifyPortRef;
    // notifier object, used to deregister later
    io_object_t notifierObject;
    // this parameter is passed to the callback
    void* refCon;
    // the pointer back to the MacOsIdleDetector
    MacOsIdleDetector* papa;
    // idle start time
    QDateTime idleStartTime;

    static void MySleepCallBack(
        void* refCon, io_service_t service,
        natural_t messageType, void* messageArgument )
    {
        // hauahauaha!
        MacOsIdleDetector::Private* d = static_cast<MacOsIdleDetector::Private*>( refCon );
        long argument = reinterpret_cast<long>( messageArgument ); // yummy

        // the case statements trigger warnings, because those
        // constants contain old-style casts :-(
        switch (  messageType )
        {
        case kIOMessageCanSystemSleep:
            /* Idle sleep is about to kick in. This message will not
               be sent for forced sleep. Applications have a chance to
               prevent sleep by calling IOCancelPowerChange. Most
               applications should not prevent idle sleep.

               Power Management waits up to 30 seconds for you to
               either allow or deny idle sleep.  If you don't
               acknowledge this power change by calling either
               IOAllowPowerChange or IOCancelPowerChange, the system
               will wait 30 seconds then go to sleep.
            */
            qDebug() << "MacOsIdleDetector: system will go to idle sleep.";
            // we will allow idle sleep
            IOAllowPowerChange( d->root_port, argument );
            d->idleStartTime = QDateTime::currentDateTime();
            break;

        case kIOMessageSystemWillSleep:
            /* The system WILL go to sleep. If you do not call
               IOAllowPowerChange or IOCancelPowerChange to
               acknowledge this message, sleep will be delayed by 30 seconds.

               NOTE: If you call IOCancelPowerChange to deny sleep it
               returns kIOReturnSuccess, however the system WILL still
               go to sleep.
            */
            qDebug() << "MacOsIdleDetector: system will go to sleep.";
            IOAllowPowerChange( d->root_port, argument );
            d->idleStartTime = QDateTime::currentDateTime();
            break;

        case kIOMessageSystemWillPowerOn:
            qDebug() << "MacOsIdleDetector: system has started the wake up process.";
            break;

        case kIOMessageSystemHasPoweredOn:
            qDebug() << "MacOsIdleDetector: system finished waking up.";
            d->papa->idle();
            break;

        default:
            break;
        }
    }

    static OSStatus sessionEventsHandler ( EventHandlerCallRef nextHandler, EventRef event, void* refCon )
    {
        // hauahauaha!
        MacOsIdleDetector::Private* d = static_cast<MacOsIdleDetector::Private*>( refCon );

        if ( GetEventKind( event ) == kEventSystemDisplaysAsleep ) {
            qDebug() << "MacOsIdleDetector: system displays go to sleep";
            d->idleStartTime = QDateTime::currentDateTime();
        } else if ( GetEventKind( event ) == kEventSystemDisplaysAwake ) {
            qDebug() << "MacOsIdleDetector: system displays woke up";
            d->papa->idle();
        }
        return noErr;
    }
};

MacOsIdleDetector::MacOsIdleDetector( QObject* parent )
    : IdleDetector( parent )
    , d( new Private )
{
    d->papa = this;
    // register to receive system sleep notifications
    d->root_port = IORegisterForSystemPower(
        d,
        &d->notifyPortRef,
        Private::MySleepCallBack,
        &d->notifierObject );

    if( d->root_port == 0 )
    {
        qDebug() << "IORegisterForSystemPower failed";
        // FIXME what to do?
    }
    // add the notification port to the application runloop
    CFRunLoopAddSource(
        CFRunLoopGetCurrent(),
        IONotificationPortGetRunLoopSource( d->notifyPortRef ),
        kCFRunLoopCommonModes );

    // register handler for session events:
    EventTypeSpec switchEventTypes[2];
    switchEventTypes[0].eventClass = kEventClassSystem;
    switchEventTypes[0].eventKind = kEventSystemDisplaysAsleep;
    switchEventTypes[1].eventClass = kEventClassSystem;
    switchEventTypes[1].eventKind = kEventSystemDisplaysAwake;
    EventHandlerUPP sessionEventsHandler = NewEventHandlerUPP( MacOsIdleDetector::Private::sessionEventsHandler );
    OSStatus err = InstallApplicationEventHandler( sessionEventsHandler, 2, switchEventTypes,  d,  NULL );
    if ( err != 0 ) {
        qDebug() << "Warning: cannot install session events handler:" << err;
    }
}

MacOsIdleDetector::~MacOsIdleDetector()
{
    // we no longer want sleep notifications:
    // remove the sleep notification port from the application runloop
    CFRunLoopRemoveSource(
        CFRunLoopGetCurrent(),
        IONotificationPortGetRunLoopSource( d->notifyPortRef ),
        kCFRunLoopCommonModes );
    // deregister for system sleep notifications
    IODeregisterForSystemPower(  &d->notifierObject );
    // IORegisterForSystemPower implicitly opens the Root Power Domain
    // IOService so we close it here
    IOServiceClose( d->root_port );
    // destroy the notification port allocated by
    // IORegisterForSystemPower
    IONotificationPortDestroy(  d->notifyPortRef );
    delete d; d = 0;
}


void MacOsIdleDetector::idle()
{
    maybeIdle( IdlePeriod( d->idleStartTime, QDateTime::currentDateTime() ) );
}

#include "MacOsIdleDetector.moc"

#endif

