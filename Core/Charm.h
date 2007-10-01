#ifndef CHARM_H
#define CHARM_H

#include <QString>


#include "Application.h"
#include "CharmDataModel.h"
#include "View.h"
#include "Configuration.h"

// FIXME this may have to go into some plugin configuration later:
// FIXME alsowe may need some verbose descriptors for configuration
#define CHARM_SQLITE_BACKEND_DESCRIPTOR "sqlite"
#define CHARM_MYSQL_BACKEND_DESCRIPTOR "mysql"

extern const QString MetaKey_EventsInLeafsOnly;
extern const QString MetaKey_OneEventAtATime;
extern const QString MetaKey_MainWindowGeometry;
extern const QString MetaKey_MainWindowGUIStateSelectedTask;
extern const QString MetaKey_MainWindowGUIStateExpandedTasks;

#define CONFIGURATION ( Configuration::instance() )
#define MODEL ( Application::instance().model() )
#define DATAMODEL ( MODEL.charmDataModel() )
#define VIEW ( Application::instance().view() )

// two helper functions to persist meta data:
bool boolForString( const QString& );
const QString& stringForBool( bool );

class View;
class Controller;
class CharmDataModel;

void connectControllerAndModel( Controller*, CharmDataModel* );

void connectControllerAndView( Controller*, View* );

#endif
