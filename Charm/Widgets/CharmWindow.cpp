/*
  CharmWindow.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2014-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Frank Osterfeld <frank.osterfeld@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "CharmWindow.h"
#include "ApplicationCore.h"
#include "Data.h"
#include "ViewHelpers.h"
#include "WidgetUtils.h"

#include "Commands/CommandRelayCommand.h"

#include "Core/CharmCommand.h"
#include "Core/CharmConstants.h"

#include <QAction>
#include <QKeyEvent>
#include <QKeySequence>
#include <QMenuBar>
#include <QSettings>
#include <QShortcut>
#include <QToolBar>
#include <QToolButton>

CharmWindow::CharmWindow( const QString& name, QWidget* parent )
    : QMainWindow( parent )
    , m_openCharmAction( new QAction( tr( "Open Charm" ), this ) )
    , m_showAction( new QAction( this ) )
{
    setWindowName( name );
    connect( m_openCharmAction, SIGNAL(triggered(bool)), SLOT(showView()) );
    connect( m_showAction, SIGNAL(triggered(bool)), SLOT(showView()) );
    connect( this, SIGNAL(visibilityChanged(bool)), SLOT(handleOpenCharm(bool)) );
    connect( this, SIGNAL(visibilityChanged(bool)), SLOT(handleShow(bool)) );
    m_toolBar = addToolBar( QStringLiteral("Toolbar") );
    m_toolBar->setMovable( false );

    emit visibilityChanged( false );
}

void CharmWindow::stateChanged( State )
{
    switch( ApplicationCore::instance().state() ) {
    case Connecting:
        setEnabled( false );
        restoreGuiState();
        configurationChanged();
        break;
    case Connected:
        configurationChanged();
        ApplicationCore::instance().createFileMenu( menuBar() );
        insertEditMenu();
        ApplicationCore::instance().createWindowMenu( menuBar() );
        ApplicationCore::instance().createHelpMenu( menuBar() );
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

QString CharmWindow::windowName() const
{
    return m_windowName;
}

void CharmWindow::setWindowIdentifier( const QString& id )
{
    m_windowIdentifier = id;
}

QString CharmWindow::windowIdentifier() const
{
    return m_windowIdentifier;
}

void CharmWindow::setWindowNumber( int number )
{
    m_windowNumber = number;
    delete m_shortcut;
    m_shortcut = new QShortcut( this );
    QKeySequence sequence( tr( "Ctrl+%1" ).arg( number ) );
#ifdef Q_OS_OSX
    m_shortcut->setKey( sequence );
#endif
    m_shortcut->setContext( Qt::ApplicationShortcut );
    m_showAction->setShortcut( sequence );
    connect( m_shortcut, SIGNAL(activated()), SLOT(showHideView()) );
    connect( m_shortcut, SIGNAL(activated()), SLOT(showView()) );
}

int CharmWindow::windowNumber() const
{
    return m_windowNumber;
}

QToolBar* CharmWindow::toolBar() const
{
    return m_toolBar;
}

QAction* CharmWindow::openCharmAction()
{
    return m_openCharmAction;
}

QAction* CharmWindow::showAction()
{
    return m_showAction;
}

void CharmWindow::restore()
{
    show();
}

void CharmWindow::checkVisibility()
{
    const auto visibility = isVisible();

    if (m_isVisibility != visibility) {
        m_isVisibility = visibility;
        emit visibilityChanged( m_isVisibility );
    }
}

void CharmWindow::showEvent( QShowEvent* e )
{
    checkVisibility();
    QMainWindow::showEvent( e );
}

void CharmWindow::hideEvent( QHideEvent* e )
{
    checkVisibility();
    QMainWindow::hideEvent( e );
}


void CharmWindow::sendCommand( CharmCommand* cmd )
{
    cmd->prepare();
    auto relay = new CommandRelayCommand( this );
    relay->setCommand( cmd );
    emit emitCommand( relay );
}

void CharmWindow::sendCommandRollback(CharmCommand *cmd)
{
    cmd->prepare();
    auto relay = new CommandRelayCommand( this );
    relay->setCommand( cmd );
    emit emitCommandRollback ( relay );
}

void CharmWindow::handleOpenCharm( bool visible )
{
    m_openCharmAction->setEnabled( !visible );
}

void CharmWindow::handleShow( bool visible )
{
    const QString text = tr( "Show %1" ).arg( m_windowName );
    m_showAction->setText( text );
    m_showAction->setEnabled( !visible );
}

void CharmWindow::commitCommand( CharmCommand* command )
{
    command->finalize();
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

void CharmWindow::showView( QWidget* w )
{
    w->show();
    w->raise();
    w->activateWindow();
}

bool CharmWindow::showHideView( QWidget* w )
{
    // hide or restore the view
    if ( w->isVisible() ) {
        w->hide();
        return false;
    } else {
        showView( w );
        return true;
    }
}

void CharmWindow::showView()
{
    showView( this );
}

void CharmWindow::showHideView()
{
    showHideView( this );
}

void CharmWindow::configurationChanged()
{
    WidgetUtils::updateToolButtonStyle( this );
}

void CharmWindow::saveGuiState()
{
    Q_ASSERT( !windowIdentifier().isEmpty() );
    QSettings settings;
    settings.beginGroup( windowIdentifier() );
    // save geometry
    WidgetUtils::saveGeometry( this, MetaKey_MainWindowGeometry );
    settings.setValue( MetaKey_MainWindowVisible, isVisible() );
}

void CharmWindow::restoreGuiState()
{
    const QString identifier = windowIdentifier();
    Q_ASSERT( !identifier.isEmpty() );
    // restore geometry
    QSettings settings;
    settings.beginGroup( identifier );
    WidgetUtils::restoreGeometry( this, MetaKey_MainWindowGeometry );
    // restore visibility
    if ( settings.contains( MetaKey_MainWindowVisible ) ) {
        // Time Tracking Window should always be visible
        const bool visible = ( identifier == QLatin1String("window_tracking") ) ? true : settings.value( MetaKey_MainWindowVisible ).toBool();
        setVisible( visible );
    }
}

#include "moc_CharmWindow.cpp"
