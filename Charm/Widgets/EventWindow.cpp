/*
  EventWindow.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

#include "EventWindow.h"
#include "EventView.h"
#include "ApplicationCore.h"

#include <QMenuBar>

EventWindow::EventWindow( QWidget* parent )
    : CharmWindow( tr( "Events Editor" ), parent )
    , m_eventView( new EventView( toolBar(), this ) )
{
    setWindowNumber( 2 );
    setWindowIdentifier( QLatin1String( "window_events" ) );
    setCentralWidget( m_eventView );
    setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Expanding );
    connect( m_eventView, SIGNAL(emitCommand(CharmCommand*)),
             SIGNAL(emitCommand(CharmCommand*)) );
    connect( m_eventView, SIGNAL(emitCommandRollback(CharmCommand*)),
             SIGNAL(emitCommandRollback(CharmCommand*)) );
}

EventWindow::~EventWindow()
{
}

void EventWindow::insertEditMenu()
{
    QMenu* editMenu = menuBar()->addMenu( tr( "Edit" ) );
    m_eventView->populateEditMenu( editMenu );
}

void EventWindow::configurationChanged()
{
    CharmWindow::configurationChanged();
    m_eventView->configurationChanged();
}

void EventWindow::stateChanged( State previous )
{
    CharmWindow::stateChanged( previous );
    m_eventView->stateChanged( previous );

    if ( ApplicationCore::instance().state() == Connecting ) {
        m_eventView->setModel( & ApplicationCore::instance().model() );
    }
}

void EventWindow::restore()
{
    show();
}

#include "moc_EventWindow.cpp"
