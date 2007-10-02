#include "ViewHelpers.h"

void connectControllerAndView( Controller* controller, View* view )
{
    // connect view and controller:
    // make controller process commands send by the view:
    QObject::connect( view, SIGNAL( emitCommand( CharmCommand* ) ),
                      controller, SLOT( executeCommand( CharmCommand* ) ) );
    // make view receive done commands from the controller:
    QObject::connect( controller, SIGNAL( commandCompleted( CharmCommand* ) ),
                      view, SLOT( commitCommand( CharmCommand* ) ) );
}

