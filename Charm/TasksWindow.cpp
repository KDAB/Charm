#include <QMenuBar>

#include "Application.h"
#include "TasksWindow.h"
#include "TasksView.h"

TasksWindow::TasksWindow( QWidget* parent )
    : CharmWindow( tr( "Tasks" ), parent )
    , m_tasksView( new TasksView( toolBar(), this ) )
{
    setWindowNumber( 1 );
    setWindowIdentifier( QLatin1String( "window_tasks" ) );
    setCentralWidget( m_tasksView );
    setUnifiedTitleAndToolBarOnMac( true );
    setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Expanding );
    connect( m_tasksView, SIGNAL( emitCommand( CharmCommand* ) ),
             SIGNAL( emitCommand( CharmCommand* ) ) );
    connect( m_tasksView, SIGNAL( saveConfiguration() ),
             SIGNAL( saveConfiguration() ) );
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

void TasksWindow::configurationChanged()
{
    CharmWindow::configurationChanged();
    m_tasksView->configurationChanged();
}

void TasksWindow::insertEditMenu()
{
    QMenu* editMenu = menuBar()->addMenu( tr( "Edit" ) );
    m_tasksView->populateEditMenu( editMenu);
}

#include "TasksWindow.moc"
