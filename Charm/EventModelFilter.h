/*
  EventModelFilter.h

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

#ifndef EVENTMODELFILTER_H
#define EVENTMODELFILTER_H

#include <QDate>
#include <QSortFilterProxyModel>

#include <Core/EventModelInterface.h>
#include <Core/CommandEmitterInterface.h>

#include "EventModelAdapter.h"

class CharmDataModel;

class EventModelFilter : public QSortFilterProxyModel,
                         public CommandEmitterInterface,
                         public EventModelInterface
{
    Q_OBJECT

public:
    explicit EventModelFilter( CharmDataModel*, QObject* parent = nullptr );
    virtual ~EventModelFilter();

    /** Returns the total number of seconds of all events in the model. */
    int totalDuration() const;

    // implement EventModelInterface:
    const Event& eventForIndex( const QModelIndex& ) const override;
    QModelIndex indexForEvent( const Event& ) const override;

    bool filterAcceptsRow( int srow, const QModelIndex & sparent ) const override;

    void setFilterStartDate( const QDate& date );
    void setFilterEndDate( const QDate& date );
    void setFilterTaskId( TaskId id );

    // implement CommandEmitterInterface:
    void commitCommand( CharmCommand* ) override;

    // implement to sort by event start datetime
    bool lessThan( const QModelIndex& left, const QModelIndex& right ) const override;

    QList<Event> events() const;

signals:
    void eventActivationNotice( EventId id );
    void eventDeactivationNotice( EventId id );

private:
    EventModelAdapter m_model;
    QDate m_start;
    QDate m_end;
    TaskId m_filterId;
};

#endif
