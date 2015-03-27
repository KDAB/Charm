/*
  Controller.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2007-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>
  Author: Olivier JG <olivier.de.gaalon@kdab.com>
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

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>

#include "Task.h"
#include "Event.h"
#include "ControllerInterface.h"

class StorageInterface;

class Controller : public QObject,
                   public ControllerInterface
{
    Q_OBJECT

public:
    explicit Controller( QObject* parent = nullptr );
    ~Controller();

    void stateChanged( State previous, State next ) override;
    void persistMetaData( Configuration& ) override;
    void provideMetaData( Configuration& ) override;

    bool initializeBackEnd( const QString& name ) override;
    bool connectToBackend() override;
    bool disconnectFromBackend() override;
    StorageInterface* storage() override;

    // FIXME add the add/modify/delete functions will not be slots anymore
    Event makeEvent( const Task& ) override;
    Event cloneEvent( const Event& ) override;
    bool modifyEvent( const Event& ) override;
    bool deleteEvent( const Event& ) override;

    bool addTask( const Task& parent ) override;
    bool modifyTask( const Task& ) override;
    bool deleteTask( const Task& ) override;
    bool setAllTasks( const TaskList& ) override;
    QDomDocument exportDatabasetoXml() const override;
    QString importDatabaseFromXml( const QDomDocument& ) override;

    void updateModelEventsAndTasks();

public slots:

    void executeCommand( CharmCommand* ) override;
    void rollbackCommand ( CharmCommand* ) override;

signals:
    void eventAdded( const Event& event );
    void eventModified( const Event& event );
    void eventDeleted( const Event& event );
    void allEvents( const EventList& );
    void definedTasks( const TaskList& );
    void taskAdded( const Task& );
    void taskUpdated( const Task& );
    void taskDeleted( const Task& );
    void readyToQuit();
    void currentBackendStatus( const QString& text );

    void commandCompleted( CharmCommand* );

private:
    void updateSubscriptionForTask( const Task& );

    template<class T> void loadConfigValue( const QString &key, T &configValue ) const;
    StorageInterface* m_storage;
};

#endif
