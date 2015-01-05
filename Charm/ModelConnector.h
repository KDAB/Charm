#ifndef MODELCONNECTOR_H
#define MODELCONNECTOR_H

#include "ViewFilter.h"
#include "Core/CharmDataModel.h"
#include "EventModelFilter.h"

class ModelConnector : public QObject,
                       public CommandEmitterInterface
{
    Q_OBJECT

public:
    ModelConnector();

    /** The charm data model. */
    CharmDataModel* charmDataModel();
    /** The item model the task view uses. */
    ViewFilter* taskModel();
    /** The item model the event view uses. */
    EventModelFilter* eventModel();

    // implement CommandEmitterInterface
    void commitCommand( CharmCommand* );

public slots:
    void slotMakeAndActivateEvent( const Task& );
    void slotRequestEventModification(const Event&newEvent, const Event& oldEvent);
    void slotSysTrayUpdate(const QString& toolTip, bool active);

private:
    CharmDataModel m_dataModel;

    ViewFilter m_viewFilter; // this is the filtered task model adapter

    EventModelFilter m_eventModelFilter; // owns the event model adapter
};

#endif
