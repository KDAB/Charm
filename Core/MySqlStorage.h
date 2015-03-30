/*
  MySqlStorage.h

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

#ifndef MYSQLSTORAGE_H
#define MYSQLSTORAGE_H

#include "SqlStorage.h"

#include <QSqlDatabase>

class MySqlStorage: public SqlStorage
{
public:
    struct Parameters {
        Parameters() : port( 3309), database( "Charm" ) {}
        unsigned int port;
        QString database;
        QString name;
        QString password;
        QString host;
    };

    MySqlStorage();
    virtual ~MySqlStorage();

    QSqlDatabase& database() override;

    QString description() const override;
    bool connect(Configuration&) override;
    bool disconnect() override;
    int installationId() const override;
    bool createDatabase(Configuration&) override;
    bool createDatabaseTables() override;

    static Parameters parseParameterEnvironmentVariable();
    void configure( const Parameters& );
protected:
    QString lastInsertRowFunction() const override;

private:
    QSqlDatabase m_database;
};

#endif /* MYSQLSTORAGE_H */
