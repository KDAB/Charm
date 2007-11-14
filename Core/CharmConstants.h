#ifndef CHARMCONSTANTS_H
#define CHARMCONSTANTS_H

#include <QString>

#include "CharmDataModel.h"
#include "Configuration.h"

// increment then SQL DB format changes:
#define CHARM_DATABASE_VERSION 1
// FIXME this may have to go into some plugin configuration later:
// FIXME also, we may need some verbose descriptors for configuration
#define CHARM_SQLITE_BACKEND_DESCRIPTOR "sqlite"
#define CHARM_MYSQL_BACKEND_DESCRIPTOR "mysql"

extern const QString MetaKey_EventsInLeafsOnly;
extern const QString MetaKey_OneEventAtATime;
extern const QString MetaKey_MainWindowGeometry;
extern const QString MetaKey_MainWindowGUIStateSelectedTask;
extern const QString MetaKey_MainWindowGUIStateExpandedTasks;
extern const QString MetaKey_ReportsRecentSavePath;
extern const QString MetaKey_ExportToXmlRecentSavePath;
extern const QString MetaKey_TimesheetActiveOnly;
extern const QString MetaKey_TimesheetSubscribedOnly;
extern const QString MetaKey_TimesheetRootTask;

#define CONFIGURATION ( Configuration::instance() )
#define DATAMODEL ( MODEL.charmDataModel() )

// two helper functions to persist meta data:
bool boolForString( const QString& );
const QString& stringForBool( bool );

class Controller;
class CharmDataModel;

void connectControllerAndModel( Controller*, CharmDataModel* );

// helpers:
/** A string containing hh:mm for the given duration of seconds. */
QString hoursAndMinutes( int seconds );

#endif
