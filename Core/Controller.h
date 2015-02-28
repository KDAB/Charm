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
    explicit Controller( QObject* parent = 0 );
    ~Controller();

    void stateChanged( State previous, State next );
    void persistMetaData( Configuration& );
    void provideMetaData( Configuration& );

    bool initializeBackEnd( const QString& name );
    bool connectToBackend();
    bool disconnectFromBackend();
    StorageInterface* storage();

    // FIXME add the add/modify/delete functions will not be slots anymore
    Event makeEvent( const Task& );
    Event cloneEvent( const Event& );
    bool modifyEvent( const Event& );
    bool deleteEvent( const Event& );

    bool addTask( const Task& parent );
    bool modifyTask( const Task& );
    bool deleteTask( const Task& );
    bool setAllTasks( const TaskList& );
    QDomDocument exportDatabasetoXml() const;
    QString importDatabaseFromXml( const QDomDocument& );

    void updateModelEventsAndTasks();

public slots:

    void executeCommand( CharmCommand* );
    void rollbackCommand ( CharmCommand* );

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
