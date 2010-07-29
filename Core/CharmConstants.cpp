#include <QObject>
#include <QTextStream>

#include "CharmConstants.h"
#include "Controller.h"
#include "CharmDataModel.h"

const QString MetaKey_EventsInLeafsOnly = "EventsInLeafsOnly";
const QString MetaKey_OneEventAtATime = "OneEventAtATime";
const QString MetaKey_MainWindowGeometry = "MainWindowGeometry";
const QString MetaKey_MainWindowVisible = "MainWindowVisible";
const QString MetaKey_MainWindowGUIStateSelectedTask = "MainWindowGUIStateSelectedTask";
const QString MetaKey_MainWindowGUIStateExpandedTasks = "MainWindowGUIStateExpandedTasks";
const QString MetaKey_TimeTrackerGeometry = "TimeTrackerGeometry";
const QString MetaKey_TimeTrackerVisible = "TimeTrackerVisible";
const QString MetaKey_ReportsRecentSavePath = "ReportsRecentSavePath";
const QString MetaKey_ExportToXmlRecentSavePath = "ExportToXmlSavePath";
const QString MetaKey_TimesheetSubscribedOnly = "TimesheetSubscribedOnly";
const QString MetaKey_TimesheetActiveOnly = "TimesheetActiveOnly";
const QString MetaKey_TimesheetRootTask = "TimesheetRootTask";
const QString MetaKey_LastEventEditorDateTime= "LastEventEditorDateTime";
const QString MetaKey_Key_InstallationId = "InstallationId";
const QString MetaKey_Key_UserName = "UserName";
const QString MetaKey_Key_UserId = "UserId";
const QString MetaKey_Key_LocalStorageDatabase = "LocalStorageDatabase";
const QString MetaKey_Key_LocalStorageType = "LocalStorageType";
const QString MetaKey_Key_SubscribedTasksOnly = "SubscribedTasksOnly";
const QString MetaKey_Key_TaskTrackerFontSize = "TaskTrackerFontSize";
const QString MetaKey_Key_24hEditing = "Key24hEditing";
const QString MetaKey_Key_DurationFormat = "DurationFormat";
const QString MetaKey_Key_IdleDetection = "IdleDetection";
const QString MetaKey_Key_ToolButtonStyle = "ToolButtonStyle";
const QString MetaKey_Key_ShowStatusBar = "ShowStatusBar";

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

static QString formatDecimal( double d ) {
    const QString s = QLocale::system().toString( d, 'f', 2 );
    if ( d > -10 && d < 10 ) //hack to get the hours always have two decimals: e.g. 00.50 instead of 0.50
        return QLatin1String("0") + s;
    else
        return s;
}

QString hoursAndMinutes( int duration )
{
    if ( duration == 0 ) {
        if ( CONFIGURATION.durationFormat == Configuration::Minutes )
            return QObject::tr( "00:00" );
        else
            return formatDecimal( 0.0 );
    }
    int minutes = duration / 60;
    int hours = minutes / 60;
    minutes = minutes % 60;

    if ( CONFIGURATION.durationFormat == Configuration::Minutes ) {
        QString text;
        QTextStream stream( &text );
        stream << qSetFieldWidth( 2 ) << qSetPadChar( QChar( '0' ) )
                << hours << qSetFieldWidth( 0 ) << ":" << qSetFieldWidth( 2 ) << minutes;
        return text;
    } else { //Decimal
        return formatDecimal(hours + minutes / 60  );
    }
}
