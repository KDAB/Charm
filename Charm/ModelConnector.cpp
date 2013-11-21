#include "ModelConnector.h"
#include "ViewHelpers.h"
#include "Data.h"

#include "Commands/CommandModifyEvent.h"
#include "Commands/CommandMakeAndActivateEvent.h"

ModelConnector::ModelConnector()
    : QObject()
    , m_dataModel()
    , m_viewFilter( &m_dataModel )
    , m_eventModelFilter( &m_dataModel )
{
    connect( &m_dataModel, SIGNAL( makeAndActivateEvent( const Task& ) ),
             SLOT( slotMakeAndActivateEvent( const Task& ) ) );
    connect( &m_dataModel, SIGNAL( requestEventModification( const Event&, const Event& ) ),
             SLOT( slotRequestEventModification( const Event&, const Event& ) ) );
    connect( &m_dataModel, SIGNAL( sysTrayUpdate( const QString&, bool ) ),
             SLOT( slotSysTrayUpdate( const QString&, bool ) ) );
}

CharmDataModel* ModelConnector::charmDataModel()
{
    return &m_dataModel;
}

ViewFilter* ModelConnector::taskModel()
{
    return &m_viewFilter;
}

EventModelFilter* ModelConnector::eventModel()
{
    return &m_eventModelFilter;
}

void ModelConnector::commitCommand( CharmCommand* command )
{
    if ( ! command->finalize() ) {
        qDebug() << "CharmDataModel::commitCommand:"
                 << command->metaObject()->className()
                 << "command has failed";
    }
}

void ModelConnector::slotMakeAndActivateEvent( const Task& task )
{
    // the command will call activateEvent in finalize, this will
    // notify the task view to update
    CharmCommand* command = new CommandMakeAndActivateEvent( task, this );
    VIEW.sendCommand( command );
}

void ModelConnector::slotRequestEventModification( const Event& newEvent, const Event& oldEvent )
{
    CommandModifyEvent* command = new CommandModifyEvent( newEvent, oldEvent, this );
    VIEW.sendCommand( command );
}

void ModelConnector::slotSysTrayUpdate(const QString& toolTip, bool active)
{
    TRAY.setToolTip( toolTip );
    TRAY.setIcon( active ? Data::charmTrayActiveIcon() : Data::charmTrayIcon() );
}

#include "moc_ModelConnector.cpp"
