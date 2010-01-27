#ifndef VIEWHELPERS_H
#define VIEWHELPERS_H

#include "Core/Event.h"
#include "Core/CharmConstants.h"

#include "Application.h"
#include "ModelConnector.h"

#define MODEL ( Application::instance().model() )
#define DATAMODEL ( MODEL.charmDataModel() )
#define VIEW ( Application::instance().mainView() )

void connectControllerAndView( Controller*, CharmWindow* );
EventIdList eventIdsSortedByStartTime( EventIdList );
/** Return those ids in the input list that elements of the subtree
 * under the parent task, which includes the parent task. */
EventIdList filteredBySubtree( EventIdList, TaskId parent, bool exclude=false );

#endif
