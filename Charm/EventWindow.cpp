#include <QMenuBar>

#include "Application.h"
#include "EventWindow.h"
#include "EventView.h"

#include <QToolBar>
#include <QLabel>

EventWindow::EventWindow( QWidget* parent )
    : CharmWindow( tr( "Events" ), parent )
    , m_eventView( new EventView( toolBar(), this ) )
{
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::MinimumExpanding );
    setWindowNumber( 2 );
    setWindowIdentifier( QLatin1String( "window_events" ) );
    setCentralWidget( m_eventView );
    setUnifiedTitleAndToolBarOnMac( true );
    connect( m_eventView, SIGNAL( emitCommand( CharmCommand* ) ),
             SIGNAL( emitCommand( CharmCommand* ) ) );
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

    if ( Application::instance().state() == Connecting ) {
        m_eventView->setModel( & Application::instance().model() );
    }
}

void EventWindow::restore()
{
    show();
}

#include "EventWindow.moc"
