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

protected:
    NSEvent* cocoaEventFilter( NSEvent* incomingEvent );

private:
    objc_object* m_eventMonitor;
};

#endif
