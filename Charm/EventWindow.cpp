#include "Application.h"
#include "EventWindow.h"
#include "EventView.h"

EventWindow::EventWindow( QWidget* parent )
    : CharmWindow( tr( "Events" ), parent )
    , m_eventView( new EventView( this ) )
{
    setWindowNumber( 2 );
    setWindowIdentifier( tr( "window_events" ) );
    setCentralWidget( m_eventView );
    connect( m_eventView, SIGNAL( emitCommand( CharmCommand* ) ),
             SIGNAL( emitCommand( CharmCommand* ) ) );
}

EventWindow::~EventWindow()
{
}

void EventWindow::stateChanged( State previous )
{
    CharmWindow::stateChanged( previous );
    m_eventView->stateChanged( previous );

    if ( Application::instance().state() == Connecting ) {
        m_eventView->setModel( & Application::instance().model() );
    }
}

void EventWindow::restore()
{
    show();
}

#include "EventWindow.moc"
