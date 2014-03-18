#include "ApplicationFactory.h"

#include "MacApplication.h"
#include "Application.h"

ApplicationFactory::ApplicationFactory()
{
}

QApplication* ApplicationFactory::localApplication( int &argc, char **argv )
{
#ifdef Q_OS_OSX
    return new MacApplication( argc, argv );
#endif
    return new Application( argc, argv );
}
