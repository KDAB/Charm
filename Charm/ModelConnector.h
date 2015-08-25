/*
  ModelConnector.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2007-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>
  Author: David Faure <david.faure@kdab.com>
  Author: Allen Winter <allen.winter@kdab.com>

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

#ifndef MODELCONNECTOR_H
#define MODELCONNECTOR_H

#include "ViewFilter.h"
#include "Core/CharmDataModel.h"
#include "EventModelFilter.h"

class ModelConnector : public QObject,
                       public CommandEmitterInterface
{
    Q_OBJECT

public:
    ModelConnector();

    /** The charm data model. */
    CharmDataModel* charmDataModel();
    /** The item model the task view uses. */
    ViewFilter* taskModel();
    /** The item model the event view uses. */
    EventModelFilter* eventModel();

    EventModelFilter* findEventModel();

    // implement CommandEmitterInterface
    void commitCommand( CharmCommand* ) override;

public slots:
    void slotMakeAndActivateEvent( const Task& );
    void slotRequestEventModification(const Event&newEvent, const Event& oldEvent);
    void slotSysTrayUpdate(const QString& toolTip, bool active);

private:
    CharmDataModel m_dataModel;

    ViewFilter m_viewFilter; // this is the filtered task model adapter

    EventModelFilter m_eventModelFilter; // owns the event model adapter

    EventModelFilter m_findEventModelFilter;
};

#endif
