#ifndef CHARM_VIEWMODEINTERFACE_H
#define CHARM_VIEWMODEINTERFACE_H

#include <Core/State.h>

class ModelConnector;

// FIXME obsolete, merge into CharmWindow
class ViewModeInterface
{
public:
    virtual ~ViewModeInterface() {}
    virtual void saveGuiState() = 0;
    virtual void restoreGuiState() = 0;
    virtual void stateChanged( State previous ) = 0;
    virtual void configurationChanged() = 0;
    virtual void setModel( ModelConnector* ) = 0;
};

#endif
