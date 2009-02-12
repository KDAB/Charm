#include <QKeyEvent>
#include <QAction>
#include <QMenuBar>
#include <QSettings>
#include <QShortcut>
#include <QKeySequence>
#include <Core/CharmCommand.h>
#include <Core/CharmConstants.h>

#include "ViewHelpers.h"
#include "Application.h"
#include "Data.h"
#include "Commands/CommandRelayCommand.h"
#include "CharmWindow.h"

#include <algorithm> //for_each()

CharmWindow::CharmWindow( const QString& name, QWidget* parent )
    : QMainWindow( parent )
    , m_windowName( name )
    , m_showHideAction( new QAction( this ) )
    , m_windowNumber( -1 )
    , m_shortcut( 0 )
{
    setWindowIcon( Data::charmIcon() );
    // FIXME make work with Mac menu merging
    show();
}

void CharmWindow::stateChanged( State previous )
{
    switch( Application::instance().state() ) {
    case Connecting:
        setEnabled( false );
        restoreGuiState();
        break;
    case Connected:
        configurationChanged();
        menuBar()->addMenu( & Application::instance().windowMenu() );
        setEnabled( true );
        break;
    case Disconnecting:
        setEnabled( false );
        saveGuiState();
        break;
    case ShuttingDown:
    case Dead:
    default:
        break;
    };
}

void CharmWindow::setWindowName( const QString& text )
{
    m_windowName = text;
}

const QString& CharmWindow::windowName() const
{
    return m_windowName;
}

void CharmWindow::setWindowIdentifier( const QString& id )
{
    m_windowIdentifier = id;
}

const QString& CharmWindow::windowIdentfier() const
{
    return m_windowIdentifier;
}

void CharmWindow::setWindowNumber( int number )
{   // FIXME restrict to Mac?
    m_windowNumber = number;
    delete m_shortcut;
    m_shortcut = new QShortcut( this );
    QKeySequence sequence( tr( "Ctrl+%1" ).arg( number ) );
    m_shortcut->setKey( sequence );
    connect( m_shortcut, SIGNAL( activated() ), SLOT( showHideView() ) );
}

int CharmWindow::windowNumber() const
{
    return m_windowNumber;
}

QAction* CharmWindow::showHideAction()
{
    return m_showHideAction;
}

void CharmWindow::restore()
{
    show();
}

void CharmWindow::showEvent( QShowEvent* e )
{
    m_showHideAction->setText( tr( "Hide %1 Window" ).arg( m_windowName ) );
    QMainWindow::showEvent( e );
}

void CharmWindow::hideEvent( QHideEvent* e )
{
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

void CharmWindow::configurationChanged()
{
    const QList<QToolButton*> buttons = findChildren<QToolButton *>();
    std::for_each( buttons.begin(), buttons.end(),
                   std::bind2nd( std::mem_fun( &QToolButton::setToolButtonStyle ), CONFIGURATION.toolButtonStyle ) );
//     Q_FOREACH( QToolButton* button, allToolButtons ) {
//         button->setToolButtonStyle( CONFIGURATION.toolButtonStyle );
//     }
}

void CharmWindow::saveGuiState()
{
    QSettings settings;
    // FIXME use config group made from the window name or ID or whatever
    // save geometry
    settings.setValue( MetaKey_MainWindowGeometry, saveGeometry() );
    settings.setValue( MetaKey_MainWindowVisible, isVisible() );
}

void CharmWindow::restoreGuiState()
{
    // FIXME use config group, implement generically for all CharmWindows
    // restore geometry
    QSettings settings;
    if ( settings.contains( MetaKey_MainWindowGeometry ) ) {
        // FIXME restore?
        restoreGeometry( settings.value( MetaKey_MainWindowGeometry ).toByteArray() );
    }
    // restore visibility
    if ( settings.contains( MetaKey_MainWindowVisible ) ) {
        const bool visible = settings.value( MetaKey_MainWindowVisible ).toBool();
        if ( visible ) {
            show();
        } else {
            hide();
        }
    }
}

#include "CharmWindow.moc"

