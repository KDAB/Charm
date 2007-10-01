#include "Model.h"
// temp:
// #include "modeltest.h"

Model::Model()
    : m_dataModel()
    , m_viewFilter( &m_dataModel )
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

ViewFilter* Model::taskModel()
{
    return &m_viewFilter;
}

EventModelFilter* Model::eventModel()
{
    return &m_eventModelFilter;
}

