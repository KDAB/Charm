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
    , m_iconNumber( 0 )
{
    connect( &m_dataModel, SIGNAL( makeAndActivateEvent( const Task& ) ),
             SLOT( slotMakeAndActivateEvent( const Task& ) ) );
    connect( &m_dataModel, SIGNAL( requestEventModification( const Event& ) ),
             SLOT( slotRequestEventModification( const Event& ) ) );
    connect( &m_dataModel, SIGNAL( sysTrayUpdate( const QString&, bool ) ),
             SLOT( slotSysTrayUpdate( const QString&, bool ) ) );
    connect( &m_iconTimer, SIGNAL( timeout() ),
             SLOT( slotSysTrayIconUpdate() ) );

    for (int i = 0; i < NPixmaps; ++i) {
        QString trayFile( ":/Charm/%1.png" );
#ifdef Q_WS_MAC
        trayFile.replace( "%1", "mac%1" );
#endif
        const QString nthFile = trayFile.arg( i + 1, 2, 10, QChar( '0' ) );
        m_pixmaps[i] = QPixmap( nthFile );
    }
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

void ModelConnector::slotSysTrayUpdate( const QString& tooltip, bool active )
{
    TRAY.setToolTip( tooltip );

    if (active && !m_iconTimer.isActive()) {
        slotSysTrayIconUpdate();
        m_iconTimer.start( 1000 ); // every second, as Nuno designed it
    } else if (!active) {
        slotSysTrayIconUpdate();
        m_iconTimer.stop();
    }
}

void ModelConnector::slotSysTrayIconUpdate()
{
    if ( m_dataModel.activeEventCount() ) {
        m_iconNumber = ( m_iconNumber + 1 ) % NPixmaps;
        //qDebug() << "systray icon update" << m_iconNumber;
        TRAY.setIcon( QIcon( m_pixmaps[ m_iconNumber ] ) );
    } else {
        TRAY.setIcon( Data::charmTrayIcon() );
    }
}

#include "ModelConnector.moc"
