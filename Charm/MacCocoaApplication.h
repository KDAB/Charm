#ifndef MACCOCOAAPPLICATION_H
#define MACCOCOAAPPLICATION_H

#include "MacApplication.h"

class NSEvent;
class objc_object;

class MacCocoaApplication : public MacApplication
{
    Q_OBJECT
public:
    MacCocoaApplication( int& argc, char* argv[] );
    ~MacCocoaApplication();
    void setupCocoaEventHandler() const;
    // This method to be public due to lack of friend classes in Objective-C and
    // the lack inheritance of Objective-C classes from C++ ones.
    void dockIconClickEvent();

protected:
    NSEvent* cocoaEventFilter( NSEvent* incomingEvent );

private:
    objc_object* m_eventMonitor;
    objc_object* m_dockIconClickEventHandler;
};

#endif
