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
    CharmWindow::stateChanged( previous );
    m_tasksView->stateChanged( previous );

    if ( Application::instance().state() == Connecting ) {
        m_tasksView->setModel( & Application::instance().model() );
    }
}

void TasksWindow::restore()
{
    show();
}

#include "TasksWindow.moc"
