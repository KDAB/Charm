#include "ApplicationFactory.h"

#include "MacApplication.h"
#include "Application.h"

ApplicationFactory::ApplicationFactory()
{
}

QApplication* ApplicationFactory::localApplication( int &argc, char **argv )
{
#ifdef Q_WS_MAC
    return new MacApplication( argc, argv );
#endif
    return new Application( argc, argv );
}
