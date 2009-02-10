#include "EventWindow.h"
#include "EventView.h"

EventWindow::EventWindow( QWidget* parent )
    : QMainWindow( parent )
{
    setCentralWidget( new EventView( this ) );
}

EventWindow::~EventWindow()
{
}

#include "EventWindow.moc"
