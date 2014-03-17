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
const QString MetaKey_Key_TimeTrackerFontSize = "TimeTrackerFontSize";
const QString MetaKey_Key_24hEditing = "Key24hEditing";
const QString MetaKey_Key_DurationFormat = "DurationFormat";
const QString MetaKey_Key_IdleDetection = "IdleDetection";
const QString MetaKey_Key_WarnUnuploadedTimesheets = "WarnUnuploadedTimesheets";
const QString MetaKey_Key_ToolButtonStyle = "ToolButtonStyle";
const QString MetaKey_Key_ShowStatusBar = "ShowStatusBar";

const QString TrueString( "true" );
const QString FalseString( "false" );

const QString& stringForBool( bool val )
{
    return val ? TrueString : FalseString;
}

void connectControllerAndModel( Controller* controller, CharmDataModel* model )
{
    QObject::connect( controller, SIGNAL(eventAdded(Event)),
                      model, SLOT(addEvent(Event)) );
    QObject::connect( controller, SIGNAL(eventModified(Event)),
                      model, SLOT(modifyEvent(Event)) );
    QObject::connect( controller, SIGNAL(eventDeleted(Event)),
                      model, SLOT(deleteEvent(Event)) );
    QObject::connect( controller, SIGNAL(allEvents(EventList)),
                      model, SLOT(setAllEvents(EventList)) );
    QObject::connect( controller, SIGNAL(definedTasks(TaskList)),
                      model, SLOT(setAllTasks(TaskList)) );
    QObject::connect( controller, SIGNAL(taskAdded(Task)),
                      model, SLOT(addTask(Task)) );
    QObject::connect( controller, SIGNAL(taskUpdated(Task)),
                      model, SLOT(modifyTask(Task)) );
    QObject::connect( controller, SIGNAL(taskDeleted(Task)),
                      model, SLOT(deleteTask(Task)) );
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
        return formatDecimal(hours + minutes / 60.0  );
    }
}
