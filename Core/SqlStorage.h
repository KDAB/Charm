/*
  SqlStorage.h

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

#ifndef SQLSTORAGE_H
#define SQLSTORAGE_H

#include <QString>

#include "StorageInterface.h"

class QSqlDatabase;
class QSqlQuery;
class QSqlRecord;

class SqlStorage : public StorageInterface
{
public:
    SqlStorage();
    ~SqlStorage();

    void stateChanged( State previous ) override;

    virtual QSqlDatabase& database() = 0;

    // installation id handling
    Installation createInstallation( const QString& name ) override;
    Installation getInstallation( int installationId ) override;
    bool modifyInstallation( const Installation& ) override;
    bool deleteInstallation( const Installation& ) override;

    // implement user database functions:
    User getUser ( int userid ) override;
    User makeUser( const QString& name ) override;
    bool modifyUser ( const User& user ) override;
    bool deleteUser ( const User& user ) override;

    // implement task database functions:
    TaskList getAllTasks() override;
    bool setAllTasks( const User& user, const TaskList& tasks ) override;
    bool addTask( const Task& task ) override;
    bool addTask( const Task& task, const SqlRaiiTransactor& ) override;
    Task getTask( int taskid ) override;
    bool modifyTask( const Task& task ) override;
    bool deleteTask( const Task& task ) override;
    bool deleteAllTasks() override;
    bool deleteAllTasks( const SqlRaiiTransactor& ) override;

    // implement event database functions:
    EventList getAllEvents() override;
    Event makeEvent() override;
    Event makeEvent( const SqlRaiiTransactor& ) override;
    Event getEvent( int eventid ) override;
    bool modifyEvent( const Event& event ) override;
    bool modifyEvent( const Event& event, const SqlRaiiTransactor& ) override;
    bool deleteEvent( const Event& event ) override;
    bool deleteAllEvents() override;
    bool deleteAllEvents( const SqlRaiiTransactor& ) override;

    // implement subscription management functions:
    bool addSubscription( User, Task ) override;
    bool deleteSubscription( User, Task ) override;

    // implement metadata management functions:
    bool setMetaData( const QString&,  const QString& ) override;
    bool setMetaData( const QString&,  const QString&, const SqlRaiiTransactor& );

    QString getMetaData( const QString& ) override;

    // implement import functions:
    QString setAllTasksAndEvents( const User&, const TaskList&, const EventList& ) override;

    /**
     * @throws UnsupportedDatabaseVersionException
     */
    bool verifyDatabase() override;

    virtual bool createDatabaseTables() = 0;

    // run the query and process possible errors
    static bool runQuery( QSqlQuery& );

protected:
    virtual QString lastInsertRowFunction() const = 0;

private:
    Event makeEventFromRecord( const QSqlRecord& );
    Task makeTaskFromRecord( const QSqlRecord& );
};

#endif
