#ifndef MODEL_H
#define MODEL_H

// #include "ViewFilter.h"
#include "Core/CharmDataModel.h"
#include "EventModelFilter.h"

class Model
{
public:
    Model();

    /** The charm data model. */
    CharmDataModel* charmDataModel();
    /** The item model the task view uses. */
    // FIXME TEMP_REM
    // ViewFilter* taskModel();
    /** The item model the event view uses. */
    EventModelFilter* eventModel();

private:
    CharmDataModel m_dataModel;

//     ViewFilter m_viewFilter; // this is the filtered task model adapter

    EventModelFilter m_eventModelFilter; // owns the event model adapter
};

#endif
