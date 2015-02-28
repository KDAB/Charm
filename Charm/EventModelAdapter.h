/*
  EventModelAdapter.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2007-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>
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

#ifndef EVENTMODELADAPTER_H
#define EVENTMODELADAPTER_H

#include <QAbstractItemModel>
#include <QPointer>

#include "Core/Event.h"
#include "Core/EventModelInterface.h"
#include "Core/CharmDataModelAdapterInterface.h"
#include "Core/CommandEmitterInterface.h"

class CharmDataModel;

class EventModelAdapter : public QAbstractListModel,
                          public CharmDataModelAdapterInterface,
                          public CommandEmitterInterface,
                          public EventModelInterface
{
    Q_OBJECT

public:
    explicit EventModelAdapter( CharmDataModel* parent );
    virtual ~EventModelAdapter();

    int rowCount( const QModelIndex& parent = QModelIndex() ) const;

    QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const;

    // reimplement CharmDataModelAdapterInterface:
    void resetTasks() {}
    void taskAboutToBeAdded( TaskId parentTask, int pos ) {}
    void taskAdded( TaskId id ) {}
    void taskModified( TaskId id ) {}
    void taskParentChanged( TaskId, TaskId, TaskId ) {}
    void taskAboutToBeDeleted( TaskId ) {}
    void taskDeleted( TaskId id ) {}

    void resetEvents();
    void eventAboutToBeAdded( EventId id );
    void eventAdded( EventId id );
    void eventModified( EventId id, Event );
    void eventAboutToBeDeleted( EventId id );
    void eventDeleted( EventId id );

    void eventActivated( EventId id );
    void eventDeactivated( EventId id );

    // reimplement EventModelInterface:
    const Event& eventForIndex( const QModelIndex& index ) const;
    QModelIndex indexForEvent( const Event& ) const;

    // reimplement CommandEmitterInterface:
    void commitCommand( CharmCommand* );

signals:
    void eventActivationNotice( EventId id );
    void eventDeactivationNotice( EventId id );

private:
    // if this is slow, we may want to store pointers here:
    EventIdList m_events;
    QPointer<CharmDataModel> m_dataModel;
};

#endif
