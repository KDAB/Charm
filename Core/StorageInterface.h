/*
  StorageInterface.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2007-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>

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

#ifndef STORAGEINTERFACE_H
#define STORAGEINTERFACE_H

#include <QString>

#include "Task.h"
#include "User.h"
#include "State.h"
#include "Event.h"
#include "Installation.h"
#include "CharmExceptions.h"

class Event;
class Configuration;
class SqlRaiiTransactor;

class StorageInterface
{
public:
    virtual ~StorageInterface()
    {
    }

    // a readable description for the user
    virtual QString description() const = 0;

    // application:
    virtual void stateChanged(State previous) = 0;

    // backend availability
    virtual bool connect(Configuration&) = 0;
    virtual bool disconnect() = 0;

    // installation id table:
    // get the id of this installation
    virtual int installationId() const = 0;
    // create an installation id
    virtual Installation getInstallation(int installationId) = 0;
    virtual Installation createInstallation(const QString& name) = 0;
    virtual bool modifyInstallation(const Installation&) = 0;
    virtual bool deleteInstallation(const Installation&) = 0;

    // user database functions:
    virtual User getUser(int userid) = 0;
    virtual User makeUser(const QString& name) = 0;
    virtual bool modifyUser(const User& user) = 0;
    virtual bool deleteUser(const User& user) = 0;

    // task database functions:
    virtual TaskList getAllTasks() = 0;
    virtual bool setAllTasks( const User& user, const TaskList& tasks ) = 0;
    virtual bool addTask(const Task& task) = 0;
    virtual bool addTask( const Task& task, const SqlRaiiTransactor& ) = 0;
    virtual Task getTask(int taskId) = 0;
    virtual bool modifyTask(const Task& task) = 0;
    virtual bool deleteTask(const Task& task) = 0;
    virtual bool deleteAllTasks() = 0;
    virtual bool deleteAllTasks( const SqlRaiiTransactor& ) = 0;

    // event database functions:
    virtual EventList getAllEvents() = 0;
    // all events are created by the storage interface
    virtual Event makeEvent() = 0;
    virtual Event makeEvent( const SqlRaiiTransactor& ) = 0;
    virtual Event getEvent(int eventId)= 0;
    virtual bool modifyEvent( const Event& event ) = 0;
    virtual bool modifyEvent( const Event& event, const SqlRaiiTransactor& ) = 0;
    virtual bool deleteEvent(const Event& event) = 0;
    virtual bool deleteAllEvents() = 0;
    virtual bool deleteAllEvents( const SqlRaiiTransactor& ) = 0;

    // subscription management functions
    // (subscriptions cannot be modified, they are just boolean flags)
    // (subscription status is retrieved with the tasks)
    virtual bool addSubscription(User, Task) = 0;
    virtual bool deleteSubscription(User, Task) = 0;

    // database metadata management functions
    virtual bool setMetaData(const QString& key, const QString& value) = 0;
    virtual QString getMetaData(const QString& key) = 0;

    /*! @brief update all tasks and events in a single-transaction during imports
      @return an empty String on success, an error message otherwise
      */
    virtual QString setAllTasksAndEvents( const User&, const TaskList&, const EventList& ) = 0;

protected:
    // Put the basic database structure into the database.
    // This includes creating the tables et cetera.
    // Different backends will have to reimplement this function to
    // get special requirements in.
    // return true if successful, false otherwise
    virtual bool createDatabase(Configuration&) = 0;
    /** Verify database content and database version.
     * Will return false if the database is found, but for some reason does not contain
     * the complete structure (which is a very unusual odd case).
     * It will throw an UnsupportedDatabaseVersionException if the database version does
     * not match the one the client was compiled against.
     */
    virtual bool verifyDatabase() = 0;
};

#endif
