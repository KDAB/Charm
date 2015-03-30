/*
  Database.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2008-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

#ifndef DATABASE_H
#define DATABASE_H

#include "Exceptions.h"

#include "Core/User.h"
#include "Core/Task.h"
#include "Core/MySqlStorage.h"

#include <QString>

class SqlRaiiTransactor;

class Database
{
public:
    Database();
    virtual ~Database();

    void login() throw ( TimesheetProcessorException );
    void initializeDatabase() throw ( TimesheetProcessorException );
    void addEvent( const Event& event, const SqlRaiiTransactor& );
    void deleteEventsForReport ( int userid, int index );
    void checkUserid( int id ) throw (TimesheetProcessorException );
    User getOrCreateUserByName( QString name ) throw (TimesheetProcessorException );
    Task getTask( int taskid ) throw (TimesheetProcessorException );
    TaskList getAllTasks() throw (TimesheetProcessorException );

    QSqlDatabase& database();

private:
    MySqlStorage m_storage;
};

#endif /*DATABASE_H*/
