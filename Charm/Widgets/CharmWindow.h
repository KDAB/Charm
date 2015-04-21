/*
  CharmWindow.h

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

#ifndef CHARMWINDOW_H
#define CHARMWINDOW_H

#include <QMainWindow>

#include "Core/ViewInterface.h"
#include "Core/CommandEmitterInterface.h"

class QAction;
class QShortcut;

class CharmWindow : public QMainWindow,
                    public ViewInterface,
                    public CommandEmitterInterface
{
    Q_OBJECT

public:
    explicit CharmWindow( const QString& name, QWidget* parent = nullptr );

    QAction* showHideAction();
    QAction* showAction();
    QAction* openCharmAction();

    QString windowName() const;
    QString windowIdentfier() const;
    int windowNumber() const;

    virtual QToolBar* toolBar() const;

protected:
    /** The window name is the human readable name the application uses to reference the window.
     */
    void setWindowName( const QString& name );
    /** The window identifier is used to reference window specific configuration groups, et cetera.
     * It is generally not recommend to change it once the application is in use. */
    void setWindowIdentifier( const QString& id );
    /** The window number is a Mac concept that allows to pull up application windows by entering CMD+<number>.
     */
    void setWindowNumber( int number );
    /** Insert the Edit menu. Empty by default. */
    virtual void insertEditMenu() {}

public:
    void stateChanged( State previous ) override;
    void showEvent( QShowEvent* ) override;
    void hideEvent( QHideEvent* ) override;
    void keyPressEvent( QKeyEvent* event ) override;

    virtual void saveGuiState();
    virtual void restoreGuiState();

    static void showView( QWidget* w );
    static bool showHideView( QWidget* w );

signals:
    void visibilityChanged( bool ) override;
    void saveConfiguration() override;

public slots:
    void sendCommandRollback( CharmCommand* ) override;
    void sendCommand( CharmCommand* ) override;
    void commitCommand( CharmCommand* ) override;
    void restore() override;
    void showView();
    void showHideView();
    void configurationChanged() override;

private:
    void handleOpenCharm( bool visible );
    void handleShow( bool visible );
    void handleShowHide( bool visible );
    QString m_windowName;
    QAction* m_openCharmAction;
    QAction* m_showAction;
    QAction* m_showHideAction;
    int m_windowNumber; // Mac numerical window number, used for shortcut etc
    QString m_windowIdentifier;
    QShortcut* m_shortcut;
    QToolBar* m_toolBar;
};

#endif
