#include <QMenuBar>

#include "ApplicationCore.h"
#include "TasksWindow.h"
#include "TasksView.h"

TasksWindow::TasksWindow( QWidget* parent )
    : CharmWindow( tr( "Tasks Editor" ), parent )
    , m_tasksView( new TasksView( toolBar(), this ) )
{
    setWindowNumber( 1 );
    setWindowIdentifier( QLatin1String( "window_tasks" ) );
    setCentralWidget( m_tasksView );
    setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Expanding );
    connect( m_tasksView, SIGNAL( emitCommand( CharmCommand* ) ),
             SIGNAL( emitCommand( CharmCommand* ) ) );
    connect( m_tasksView, SIGNAL( emitCommandRollback( CharmCommand* ) ),
             SIGNAL( emitCommandRollback( CharmCommand* ) ) );
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

    if ( ApplicationCore::instance().state() == Connecting ) {
        m_tasksView->setModel( & ApplicationCore::instance().model() );
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

void TasksWindow::sendCommand( CharmCommand* )
{
    Q_ASSERT( false ); // should not be called
}

void TasksWindow::sendCommandRollback( CharmCommand* )
{
    Q_ASSERT( false ); // should not be called
}

void TasksWindow::commitCommand( CharmCommand* )
{
}

#include "moc_TasksWindow.cpp"
