#include "Data.h"
#include "Application.h"
#include "TasksWindow.h"
#include "TasksView.h"

TasksWindow::TasksWindow( QWidget* parent )
    : CharmWindow( parent )
    , m_tasksView( new View( this ) )
{
    setCentralWidget( m_tasksView );
    setWindowIcon( Data::charmIcon() );
}

TasksWindow::~TasksWindow()
{
}

void TasksWindow::stateChanged( State previous )
{
    m_tasksView->setModel( & Application::instance().model() );
    m_tasksView->stateChanged( previous );

    switch( Application::instance().state() ) {
    case Connected:
        // slotConfigurationChanged();
        setEnabled( true );
        break;
    case Disconnecting:
        setEnabled( false );
        // saveGuiState();
        break;
    case Connecting:
        setEnabled( false );
        // restoreGuiState();
        break;
    case ShuttingDown:
    case Dead:
    case StartingUp:
    default:
        break;
    };
}

void TasksWindow::restore()
{
    show();
}

#include "TasksWindow.moc"
