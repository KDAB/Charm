#include "ViewHelpers.h"

void connectControllerAndView( Controller* controller, MainWindow* view )
{
    // connect view and controller:
    // make controller process commands send by the view:
    QObject::connect( view, SIGNAL( emitCommand( CharmCommand* ) ),
                      controller, SLOT( executeCommand( CharmCommand* ) ) );
    // make view receive done commands from the controller:
    QObject::connect( controller, SIGNAL( commandCompleted( CharmCommand* ) ),
                      view, SLOT( commitCommand( CharmCommand* ) ) );
    // window title updates
    QObject::connect( controller, SIGNAL( currentBackendStatus( const QString& ) ),
                      view, SLOT( slotCurrentBackendStatusChanged( const QString& ) ) );

}

