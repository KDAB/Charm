#ifndef VIEWHELPERS_H
#define VIEWHELPERS_H

#include "Core/Event.h"
#include "Core/CharmConstants.h"

#include "ApplicationCore.h"
#include "ModelConnector.h"

#define MODEL ( ApplicationCore::instance().model() )
#define DATAMODEL ( MODEL.charmDataModel() )
#define VIEW ( ApplicationCore::instance().mainView() )
#define TRAY ( ApplicationCore::instance().trayIcon() )

void connectControllerAndView( Controller*, CharmWindow* );
EventIdList eventIdsSortedByStartTime( EventIdList );
/** Return those ids in the input list that elements of the subtree
 * under the parent task, which includes the parent task. */
EventIdList filteredBySubtree( EventIdList, TaskId parent, bool exclude=false );
QString elidedTaskName( const QString& text, const QFont& font, int width );

#endif
