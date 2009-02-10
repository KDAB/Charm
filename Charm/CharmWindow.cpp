#include <QKeyEvent>
#include <QAction>

#include <Core/CharmCommand.h>

#include "Application.h"
#include "Data.h"
#include "Commands/CommandRelayCommand.h"
#include "CharmWindow.h"

CharmWindow::CharmWindow( const QString& name, QWidget* parent )
    : QMainWindow( parent )
    , m_windowName( name )
    , m_showHideAction( new QAction( this ) )
{
    setWindowIcon( Data::charmIcon() );
}

void CharmWindow::stateChanged( State previous )
{
    switch( Application::instance().state() ) {
    case Connecting:
        setEnabled( false );
        // restoreGuiState();
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



QAction* CharmWindow::showHideAction()
{
    return m_showHideAction;
}

void CharmWindow::showEvent( QShowEvent* e )
{
    emit visibilityChanged( true );
    m_showHideAction->setText( tr( "Hide %1 Window" ).arg( m_windowName ) );
    QMainWindow::showEvent( e );
}

void CharmWindow::hideEvent( QHideEvent* e )
{
    emit visibilityChanged( false );
    m_showHideAction->setText( tr( "Show %1 Window" ).arg( m_windowName ) );
    QMainWindow::hideEvent( e );
}


void CharmWindow::sendCommand( CharmCommand* cmd )
{
    cmd->prepare();
    CommandRelayCommand* relay = new CommandRelayCommand( this );
    relay->setCommand( cmd );
    emit emitCommand( relay );
}

void CharmWindow::commitCommand( CharmCommand* command )
{
    command->finalize();
    delete command;
}

void CharmWindow::keyPressEvent( QKeyEvent* event )
{
    if ( event->type() == QEvent::KeyPress ) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>( event );
        if ( keyEvent->modifiers() & Qt::ControlModifier
             && keyEvent->key() == Qt::Key_W ) {
            // we must be visible, otherwise we would not get the event
            showHideView();
        }
    }
    QMainWindow::keyPressEvent( event );
}

void CharmWindow::showHideView()
{
    // hide or restore the view
    if ( isVisible() ) {
        hide();
    } else {
        show();
        restore();
        raise();
    }
}


#include "CharmWindow.moc"

