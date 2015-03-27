/*
  SqLiteStorage.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2007-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>
  Author: Mike McQuaid <mike.mcquaid@kdab.com>

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

#ifndef SQLITESTORAGE_H
#define SQLITESTORAGE_H

#include <QSqlDatabase>
#include <QDir>

#include "SqlStorage.h"

class Configuration;

class SqLiteStorage : public SqlStorage
{
public:
    SqLiteStorage();
    ~SqLiteStorage();

    QString description() const override;
    bool connect( Configuration& ) override;
    bool disconnect() override;

    QSqlDatabase& database() override;
    int installationId() const override;

protected:
    bool createDatabase( Configuration& ) override;
    bool createDatabaseTables() override;
    bool migrateDatabaseDirectory(QDir, const QDir & ) const;
    QString lastInsertRowFunction() const override;

private:
    QSqlDatabase m_database;
    int m_installationId;
};

#endif
