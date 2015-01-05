#include <QMenuBar>

#include "ApplicationCore.h"
#include "EventWindow.h"
#include "EventView.h"


EventWindow::EventWindow( QWidget* parent )
    : CharmWindow( tr( "Events Editor" ), parent )
    , m_eventView( new EventView( toolBar(), this ) )
{
    setWindowNumber( 2 );
    setWindowIdentifier( QLatin1String( "window_events" ) );
    setCentralWidget( m_eventView );
    setUnifiedTitleAndToolBarOnMac( true );
    setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Expanding );
    connect( m_eventView, SIGNAL( emitCommand( CharmCommand* ) ),
             SIGNAL( emitCommand( CharmCommand* ) ) );
    connect( m_eventView, SIGNAL( emitCommandRollback( CharmCommand* ) ),
             SIGNAL( emitCommandRollback( CharmCommand* ) ) );
}

EventWindow::~EventWindow()
{
}

void EventWindow::insertEditMenu()
{
    QMenu* editMenu = menuBar()->addMenu( tr( "Edit" ) );
    m_eventView->populateEditMenu( editMenu );
}

void EventWindow::configurationChanged()
{
    CharmWindow::configurationChanged();
    m_eventView->configurationChanged();
}

void EventWindow::stateChanged( State previous )
{
    CharmWindow::stateChanged( previous );
    m_eventView->stateChanged( previous );

    if ( ApplicationCore::instance().state() == Connecting ) {
        m_eventView->setModel( & ApplicationCore::instance().model() );
    }
}

void EventWindow::restore()
{
    show();
}

#include "moc_EventWindow.cpp"
