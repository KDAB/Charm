#ifndef MACCARBONAPPLICATION_H
#define MACCARBONAPPLICATION_H

#include "MacApplication.h"

class MacCarbonApplication : public MacApplication
{
    Q_OBJECT
public:
    MacCarbonApplication( int& argc, char* argv[] );
    ~MacCarbonApplication();

protected:
    bool macEventFilter( EventHandlerCallRef caller, EventRef event );
};

#endif
