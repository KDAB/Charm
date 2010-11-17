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
    connect( &m_dataModel, SIGNAL( requestEventModification( const Event& ) ),
             SLOT( slotRequestEventModification( const Event& ) ) );
    connect( &m_dataModel, SIGNAL( sysTrayUpdate( const QString&, bool, int ) ),
             SLOT( slotSysTrayUpdate( const QString&, bool, int ) ) );
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
    delete command;
}

void ModelConnector::slotMakeAndActivateEvent( const Task& task )
{
    // the command will call activateEvent in finalize, this will
    // notify the task view to update
    CharmCommand* command = new CommandMakeAndActivateEvent( task, this );
    VIEW.sendCommand( command );
}

void ModelConnector::slotRequestEventModification( const Event& event )
{
    CommandModifyEvent* command = new CommandModifyEvent( event, this );
    VIEW.sendCommand( command );
}

void ModelConnector::slotSysTrayUpdate( const QString& tooltip, bool active, int duration )
{
    TRAY.setToolTip( tooltip );

#if !defined Q_WS_MAC
//TODO: port the nth minute icons to mac
    if ( active ) {
        //duration is in seconds
        const int mins = duration / 60;
        const int nth = ( mins % 60 ) / 5 + 1;
        QString nthFile = QString( ":/Charm/%1.png" ).arg( nth, 2, 10, QChar( '0' ) );
        TRAY.setIcon( QIcon( QPixmap( nthFile ) ) );
    } else {
        TRAY.setIcon( Data::charmTrayIcon() );
    }
#endif
}

#include "ModelConnector.moc"
