#ifndef CHARMCONSTANTS_H
#define CHARMCONSTANTS_H

#include <QString>

#include "CharmDataModel.h"
#include "Configuration.h"

// increment when SQL DB format changes:
#define CHARM_DATABASE_VERSION_DESCRIPTOR "CharmDatabaseSchemaVersion"
#define CHARM_DATABASE_VERSION_BEFORE_TASK_EXPIRY 2
#define CHARM_DATABASE_VERSION_BEFORE_TRACKABLE 3
#define CHARM_DATABASE_VERSION 4
#define REQUIRED_CHARM_DATABASE_VERSION CHARM_DATABASE_VERSION
// FIXME this may have to go into some plugin configuration later:
// FIXME also, we may need some verbose descriptors for configuration
#define CHARM_SQLITE_BACKEND_DESCRIPTOR "sqlite"
#define CHARM_MYSQL_BACKEND_DESCRIPTOR "mysql"

// Metadata and QSettings Keys:
extern const QString MetaKey_MainWindowGeometry;
extern const QString MetaKey_MainWindowVisible;
extern const QString MetaKey_MainWindowGUIStateSelectedTask;
extern const QString MetaKey_MainWindowGUIStateExpandedTasks;
extern const QString MetaKey_TimeTrackerGeometry;
extern const QString MetaKey_TimeTrackerVisible;
extern const QString MetaKey_ReportsRecentSavePath;
extern const QString MetaKey_ExportToXmlRecentSavePath;
extern const QString MetaKey_TimesheetActiveOnly;
extern const QString MetaKey_TimesheetSubscribedOnly;
extern const QString MetaKey_TimesheetRootTask;
extern const QString MetaKey_LastEventEditorDateTime;
extern const QString MetaKey_Key_InstallationId;
extern const QString MetaKey_Key_UserName;
extern const QString MetaKey_Key_UserId;
extern const QString MetaKey_Key_LocalStorageDatabase;
extern const QString MetaKey_Key_LocalStorageType;
extern const QString MetaKey_Key_SubscribedTasksOnly;
extern const QString MetaKey_Key_TimeTrackerFontSize;
extern const QString MetaKey_Key_DurationFormat;
extern const QString MetaKey_Key_IdleDetection;
extern const QString MetaKey_Key_WarnUnuploadedTimesheets;
extern const QString MetaKey_Key_ToolButtonStyle;
extern const QString MetaKey_Key_ShowStatusBar;

extern const QString TrueString;
extern const QString FalseString;

#define CONFIGURATION ( Configuration::instance() )

// helper functions to persist meta data:
template<class T> T strToT( const QString &str );
template<> inline int strToT( const QString& str )
{
    bool ok;
    int ret = str.toInt( &ok );
    Q_ASSERT( ok ); Q_UNUSED( ok );
    return ret;
}
template<> inline bool strToT( const QString& str )
{
    return str.simplified() == TrueString;
}
#define INT_CONFIG_TYPE( TYPE )\
template<> inline TYPE strToT( const QString& str )\
{ return static_cast<TYPE>( strToT<int> ( str ) ); }
INT_CONFIG_TYPE( Configuration::TimeTrackerFontSize )
INT_CONFIG_TYPE( Configuration::DurationFormat )
INT_CONFIG_TYPE( Configuration::TaskPrefilteringMode )
INT_CONFIG_TYPE( Qt::ToolButtonStyle )

const QString &stringForBool( bool val );

class Controller;
class CharmDataModel;

void connectControllerAndModel( Controller*, CharmDataModel* );

// helpers:
/** A string containing hh:mm for the given duration of seconds. */
QString hoursAndMinutes( int seconds );

#endif
