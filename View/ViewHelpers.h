#ifndef VIEWHELPERS_H
#define VIEWHELPERS_H

#include "Core/CharmConstants.h"

#include "Application.h"
#include "Model.h"

#define MODEL ( Application::instance().model() )
#define DATAMODEL ( MODEL.charmDataModel() )
#define VIEW ( Application::instance().view() )

void connectControllerAndView( Controller*, View* );

#endif
