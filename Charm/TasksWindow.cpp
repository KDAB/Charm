#include "Application.h"
#include "TasksWindow.h"
#include "TasksView.h"

TasksWindow::TasksWindow( QWidget* parent )
    : CharmWindow( tr( "Tasks" ), parent )
    , m_tasksView( new View( this ) )
{
    setCentralWidget( m_tasksView );
    connect( m_tasksView, SIGNAL( emitCommand( CharmCommand* ) ),
             SLOT( sendCommand( CharmCommand* ) ) );
}

TasksWindow::~TasksWindow()
{
}

void TasksWindow::stateChanged( State previous )
{
    // FIXME refactor to base class
    m_tasksView->stateChanged( previous );

    switch( Application::instance().state() ) {
    case Connecting:
        setEnabled( false );
        // restoreGuiState();
        m_tasksView->setModel( & Application::instance().model() );
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

void TasksWindow::restore()
{
    show();
}

#include "TasksWindow.moc"
