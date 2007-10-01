#include <QObject>

#include "View.h"
#include "Charm.h"
#include "Controller.h"
#include "CharmDataModel.h"

const QString MetaKey_EventsInLeafsOnly = "EventsInLeafsOnly";
const QString MetaKey_OneEventAtATime = "OneEventAtATime";
const QString MetaKey_MainWindowGeometry = "MainWindowGeometry";
const QString MetaKey_MainWindowGUIStateSelectedTask = "MainWindowGUIStateSelectedTask";
const QString MetaKey_MainWindowGUIStateExpandedTasks = "MainWindowGUIStateExpandedTasks";

const QString TrueString( "true" );
const QString FalseString( "false" );


const QString& stringForBool ( bool value )
{
    if ( value ) {
        return TrueString;
    } else {
        return FalseString;
    }
}

bool boolForString( const QString& text )
{
    if ( text.simplified() == TrueString ) {
        return true;
    } else {
        return false;
    }
}

void connectControllerAndModel( Controller* controller, CharmDataModel* model )
{
    QObject::connect( controller, SIGNAL( eventAdded( const Event& ) ),
                      model, SLOT( addEvent( const Event& ) ) );
    QObject::connect( controller, SIGNAL( eventModified( const Event& ) ),
                      model, SLOT( modifyEvent( const Event& ) ) );
    QObject::connect( controller, SIGNAL( eventDeleted( const Event& ) ),
                      model, SLOT( deleteEvent( const Event& ) ) );
    QObject::connect( controller, SIGNAL( allEvents( const EventList& ) ),
                      model, SLOT( setAllEvents( const EventList& ) ) );
    QObject::connect( controller, SIGNAL( definedTasks( const TaskList& ) ),
                      model, SLOT( setAllTasks( const TaskList& ) ) );
    QObject::connect( controller, SIGNAL( taskAdded( const Task& ) ),
                      model, SLOT( addTask( const Task& ) ) );
    QObject::connect( controller, SIGNAL( taskUpdated( const Task& ) ),
                      model, SLOT( modifyTask( const Task& ) ) );
    QObject::connect( controller, SIGNAL( taskDeleted( const Task& ) ),
                      model, SLOT( deleteTask( const Task& ) ) );
}

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

