#include "Application.h"
#include "EventWindow.h"
#include "EventView.h"

EventWindow::EventWindow( QWidget* parent )
    : CharmWindow( tr( "Events" ), parent )
    , m_eventView( new EventView( this ) )
{
    setCentralWidget( m_eventView );
    connect( m_eventView, SIGNAL( emitCommand( CharmCommand* ) ),
             SLOT( sendCommand( CharmCommand* ) ) );
}

EventWindow::~EventWindow()
{
}

void EventWindow::stateChanged( State previous )
{
    // FIXME refactor to base class
    m_eventView->stateChanged( previous );

    switch( Application::instance().state() ) {
    case Connecting:
        setEnabled( false );
        // restoreGuiState();
        m_eventView->setModel( & Application::instance().model() );
        break;
    case Connected:
        // slotConfigurationChanged();
        setEnabled( true );
        break;
    case Disconnecting:
        setEnabled( false );
        // saveGuiState();
        break;
    case ShuttingDown:
    case Dead:
    default:
        break;
    };
}

void EventWindow::restore()
{
    show();
}

#include "EventWindow.moc"
