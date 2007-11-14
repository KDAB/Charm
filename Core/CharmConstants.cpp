#include <QObject>

#include "CharmConstants.h"
#include "Controller.h"
#include "CharmDataModel.h"

const QString MetaKey_EventsInLeafsOnly = "EventsInLeafsOnly";
const QString MetaKey_OneEventAtATime = "OneEventAtATime";
const QString MetaKey_MainWindowGeometry = "MainWindowGeometry";
const QString MetaKey_MainWindowGUIStateSelectedTask = "MainWindowGUIStateSelectedTask";
const QString MetaKey_MainWindowGUIStateExpandedTasks = "MainWindowGUIStateExpandedTasks";
const QString MetaKey_ReportsRecentSavePath = "ReportsRecentSavePath";
const QString MetaKey_ExportToXmlRecentSavePath = "ExportToXmlSavePath";
const QString MetaKey_TimesheetSubscribedOnly = "TimesheetSubscribedOnly";
const QString MetaKey_TimesheetActiveOnly = "TimesheetActiveOnly";
const QString MetaKey_TimesheetRootTask = "TimesheetRootTask";

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

QString hoursAndMinutes( int duration )
{
    if ( duration > 0 )
    {
        int minutes = duration / 60;
        int hours = minutes / 60;
        minutes = minutes % 60;

        return QString( "%1:%2" ).
            arg( hours, 2, 10, QChar( '0' ) ).
            arg( minutes, 2, 10, QChar( '0' ) );
    } else {
        return QObject::tr( "00:00" );
    }
}

