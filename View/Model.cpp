#include "Model.h"
// temp:
// #include "modeltest.h"

Model::Model()
    : m_dataModel()
// FIXME TEMP_REM
//    , m_viewFilter( &m_dataModel )
    , m_eventModelFilter( &m_dataModel )
{
// #ifndef NDEBUG
//     new Modeltest( taskModel() );
// #endif
}

CharmDataModel* Model::charmDataModel()
{
    return &m_dataModel;
}

// FIXME TEMP_REM
/*
ViewFilter* Model::taskModel()
{
    return &m_viewFilter;
}
*/

EventModelFilter* Model::eventModel()
{
    return &m_eventModelFilter;
}

