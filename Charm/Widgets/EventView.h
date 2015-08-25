/*
  EventView.h

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

#ifndef EVENTVIEW_H
#define EVENTVIEW_H

#include <QWidget>
#include <QAction>
#include <QUndoStack>

#include "Core/Event.h"
#include "Core/TimeSpans.h"
#include "Core/CommandEmitterInterface.h"

#include "ViewModeInterface.h"
#include "UndoCharmCommandWrapper.h"

class QModelIndex;

class CharmCommand;
class EventModelFilter;
class QToolBar;
class QComboBox;
class QLabel;
class QListView;

class EventView : public QWidget,
                  public ViewModeInterface,
                  public CommandEmitterInterface
{
    Q_OBJECT

public:
    explicit EventView( QToolBar* toolBar, QWidget* parent );
    ~EventView();

    void closeEvent( QCloseEvent* ) override;

    void reject();

    void makeVisibleAndCurrent( const Event& );

    // implement ViewModeInterface:
    void saveGuiState() override;
    void restoreGuiState() override;
    void stateChanged( State previous ) override;
    void configurationChanged() override;
    void setModel( ModelConnector* ) override;

    void populateEditMenu( QMenu* );

signals:
    void visible( bool );
    void emitCommand( CharmCommand* );
    void emitCommandRollback( CharmCommand* );

public slots:
    void commitCommand( CharmCommand* ) override;
    void delayedInitialization();
    void timeSpansChanged();
    void timeFrameChanged(int );
    void slotConfigureUi();

private slots:
    void slotEventDoubleClicked( const QModelIndex& );
    void slotEditEvent();
    void slotEditEvent( const Event& );
    void slotEventChangesCompleted( const Event& );
    void slotCurrentItemChanged( const QModelIndex&, const QModelIndex& );
    void slotContextMenuRequested( const QPoint& );
    void slotNextEvent();
    void slotPreviousEvent();
    void slotNewEvent();
    void slotDeleteEvent();
    void slotEventActivated( EventId );
    void slotEventDeactivated( EventId );
    void slotUpdateTotal();
    void slotUpdateCurrent();
    void slotUndoTextChanged(const QString&);
    void slotRedoTextChanged(const QString&);
    void slotEventIdChanged(int oldId, int newId);
    void slotFindAndReplace();
    void slotReset();

private:
    Event newSettings();
    void setCurrentEvent( const Event& );
    void stageCommand( CharmCommand* );

    QUndoStack* m_undoStack;
    QList<NamedTimeSpan> m_timeSpans;
    Event m_event;
    EventModelFilter* m_model;
    QAction m_actionUndo;
    QAction m_actionRedo;
    QAction m_actionNewEvent;
    QAction m_actionEditEvent;
    QAction m_actionDeleteEvent;
    QAction m_actionCreateTimeSheet;
    QAction m_actionFindAndReplace;
    QComboBox* m_comboBox;
    QLabel* m_labelTotal;
    QListView* m_listView;
};

#endif
