#include <QKeyEvent>
#include <QAction>
#include <QMenuBar>
#include <QSettings>
#include <QShortcut>
#include <QKeySequence>
#include <QToolButton>

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
    , m_showHideAction( new QAction( this ) )
    , m_windowNumber( -1 )
    , m_shortcut( 0 )
{
    setWindowName( name );
    setWindowIcon( Data::charmIcon() );
    handleShowHide( false );
    connect( m_showHideAction, SIGNAL( triggered( bool ) ), SLOT( showHideView() ) );
}

void CharmWindow::stateChanged( State )
{
    switch( Application::instance().state() ) {
    case Connecting:
        setEnabled( false );
        restoreGuiState();
        configurationChanged();
        break;
    case Connected:
        configurationChanged();
        menuBar()->addMenu( & Application::instance().fileMenu() );
        insertEditMenu();
        menuBar()->addMenu( & Application::instance().windowMenu() );
        menuBar()->addMenu( & Application::instance().helpMenu() );
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
    setWindowTitle( text );
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
    m_shortcut->setContext( Qt::ApplicationShortcut );
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
    handleShowHide( true );
    QMainWindow::showEvent( e );
}

void CharmWindow::hideEvent( QHideEvent* e )
{
    handleShowHide( false );
    QMainWindow::hideEvent( e );
}


void CharmWindow::sendCommand( CharmCommand* cmd )
{
    cmd->prepare();
    CommandRelayCommand* relay = new CommandRelayCommand( this );
    relay->setCommand( cmd );
    emit emitCommand( relay );
}

void CharmWindow::handleShowHide( bool visible )
{
    const QString text = visible ?  tr( "Hide %1 Window" ).arg( m_windowName )
        :  tr( "Show %1 Window" ).arg( m_windowName );
    m_showHideAction->setText( text );
    emit visibilityChanged( visible );
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
             && keyEvent->key() == Qt::Key_W
             && isVisible() ) {
            showHideView();
            return;
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
}

void CharmWindow::saveGuiState()
{
    Q_ASSERT( !windowIdentfier().isEmpty() );
    QSettings settings;
    settings.beginGroup( windowIdentfier() );
    // save geometry
    settings.setValue( MetaKey_MainWindowGeometry, saveGeometry() );
    settings.setValue( MetaKey_MainWindowVisible, isVisible() );
}

void CharmWindow::restoreGuiState()
{
    Q_ASSERT( !windowIdentfier().isEmpty() );
    // restore geometry
    QSettings settings;
    settings.beginGroup( windowIdentfier() );
    if ( settings.contains( MetaKey_MainWindowGeometry ) ) {
        restoreGeometry( settings.value( MetaKey_MainWindowGeometry ).toByteArray() );
    }
    // restore visibility
    if ( settings.contains( MetaKey_MainWindowVisible ) ) {
        const bool visible = settings.value( MetaKey_MainWindowVisible ).toBool();
        setVisible(visible);
    }
}

#include "CharmWindow.moc"

