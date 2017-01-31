/*
  SqlStorage.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2007-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

#ifndef SQLSTORAGE_H
#define SQLSTORAGE_H

#include <QString>

#include "Task.h"
#include "User.h"
#include "State.h"
#include "Event.h"
#include "Installation.h"
#include "CharmExceptions.h"

class QSqlDatabase;
class QSqlQuery;
class QSqlRecord;
class Configuration;
class SqlRaiiTransactor;

class SqlStorage
{
public:
    SqlStorage();
    virtual ~SqlStorage();

    // a readable description for the user
    virtual QString description() const = 0;

    // backend availability
    virtual bool connect(Configuration &) = 0;
    virtual bool disconnect() = 0;

    virtual QSqlDatabase &database() = 0;

    // installation id table:
    // get the id of this installation
    virtual int installationId() const = 0;

    // application:
    void stateChanged(State previous);

    // installation id handling
    Installation createInstallation(const QString &name);

    // create an installation id
    Installation getInstallation(int installationId);
    bool modifyInstallation(const Installation &);
    bool deleteInstallation(const Installation &);

    // user database functions:
    User getUser(int userid);
    User makeUser(const QString &name);
    bool modifyUser(const User &user);
    bool deleteUser(const User &user);

    // task database functions:
    TaskList getAllTasks();
    bool setAllTasks(const User &user, const TaskList &tasks);
    bool addTask(const Task &task);
    bool addTask(const Task &task, const SqlRaiiTransactor &);
    Task getTask(int taskid);
    bool modifyTask(const Task &task);
    bool deleteTask(const Task &task);
    bool deleteAllTasks();
    bool deleteAllTasks(const SqlRaiiTransactor &);

    // event database functions:
    EventList getAllEvents();

    // all events are created by the storage interface
    Event makeEvent();
    Event makeEvent(const SqlRaiiTransactor &);
    Event getEvent(int eventid);
    bool modifyEvent(const Event &event);
    bool modifyEvent(const Event &event, const SqlRaiiTransactor &);
    bool deleteEvent(const Event &event);
    bool deleteAllEvents();
    bool deleteAllEvents(const SqlRaiiTransactor &);

    // subscription management functions
    // (subscriptions cannot be modified, they are just boolean flags)
    // (subscription status is retrieved with the tasks)
    bool addSubscription(User, Task);
    bool deleteSubscription(User, Task);

    // implement metadata management functions:
    bool setMetaData(const QString &, const QString &);
    bool setMetaData(const QString &, const QString &, const SqlRaiiTransactor &);

    // database metadata management functions
    QString getMetaData(const QString &);

    /*! @brief update all tasks and events in a single-transaction during imports
      @return an empty String on success, an error message otherwise
      */
    QString setAllTasksAndEvents(const User &, const TaskList &, const EventList &);

    /**
     * @throws UnsupportedDatabaseVersionException
     */
    bool verifyDatabase();

    virtual bool createDatabaseTables() = 0;

    // run the query and process possible errors
    static bool runQuery(QSqlQuery &);

protected:
    // Put the basic database structure into the database.
    // This includes creating the tables et cetera.
    // Different backends will have to reimplement this function to
    // get special requirements in.
    // return true if successful, false otherwise
    virtual bool createDatabase(Configuration &) = 0;

    /** Verify database content and database version.
     * Will return false if the database is found, but for some reason does not contain
     * the complete structure (which is a very unusual odd case).
     * It will throw an UnsupportedDatabaseVersionException if the database version does
     * not match the one the client was compiled against.
     */
    virtual QString lastInsertRowFunction() const = 0;

private:
    bool migrateDB(const QString &queryString, int oldVersion);
    Event makeEventFromRecord(const QSqlRecord &);
    Task makeTaskFromRecord(const QSqlRecord &);
};

#endif
