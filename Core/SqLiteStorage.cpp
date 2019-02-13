/*
  SqLiteStorage.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2007-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>
  Author: Frank Osterfeld <frank.osterfeld@kdab.com>
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

#include "SqLiteStorage.h"
#include "CharmConstants.h"
#include "CharmExceptions.h"
#include "Configuration.h"
#include "Event.h"

#include <QDir>
#include <QtDebug>
#include <QFileInfo>
#include <QSqlDatabase>
#include <QSqlQuery>

#include <cerrno>

// DATABASE STRUCTURE DEFINITION
static const QString Tables[] = {
    QStringLiteral("MetaData"), QStringLiteral("Installations"), QStringLiteral("Tasks"),
    QStringLiteral("Events"), QStringLiteral("Subscriptions"), QStringLiteral("Users")
};

static const int NumberOfTables = sizeof Tables / sizeof Tables[0];

struct Field
{
    QString name;
    QString type;
};

typedef Field Fields;
const Field LastField =
{ QString::null, QString::null};

static const Fields MetaData_Fields[] = {
    { QStringLiteral("id"), QStringLiteral("INTEGER PRIMARY KEY") },
    { QStringLiteral("key"), QStringLiteral("VARCHAR( 128 ) NOT NULL") },
    { QStringLiteral("value"), QStringLiteral("VARCHAR( 128 )") }, LastField
};

static const Fields Installations_Fields[] = {
    { QStringLiteral("id"), QStringLiteral("INTEGER PRIMARY KEY") },
    { QStringLiteral("inst_id"), QStringLiteral("INTEGER") },
    { QStringLiteral("user_id"), QStringLiteral("INTEGER") },
    { QStringLiteral("name"), QStringLiteral("varchar(256)") }, LastField
};

static const Fields Tasks_Fields[] = {
    { QStringLiteral("id"), QStringLiteral("INTEGER PRIMARY KEY") },
    { QStringLiteral("task_id"), QStringLiteral("INTEGER UNIQUE") },
    { QStringLiteral("parent"), QStringLiteral("INTEGER") },
    { QStringLiteral("validfrom"), QStringLiteral("timestamp") },
    { QStringLiteral("validuntil"), QStringLiteral("timestamp") },
    { QStringLiteral("trackable"), QStringLiteral("INTEGER") },
    { QStringLiteral("comment"), QStringLiteral("varchar(256)")},
    { QStringLiteral("name"), QStringLiteral("varchar(256)") }, LastField
};

static const Fields Event_Fields[] = {
    { QStringLiteral("id"), QStringLiteral("INTEGER PRIMARY KEY") },
    { QStringLiteral("user_id"), QStringLiteral("INTEGER") },
    { QStringLiteral("event_id"), QStringLiteral("INTEGER") },
    { QStringLiteral("installation_id"), QStringLiteral("INTEGER") },
    { QStringLiteral("report_id"), QStringLiteral("INTEGER NULL") },
    { QStringLiteral("task"), QStringLiteral("INTEGER") },
    { QStringLiteral("comment"), QStringLiteral("varchar(256)") },
    { QStringLiteral("start"), QStringLiteral("date") },
    { QStringLiteral("end"), QStringLiteral("date") }, LastField
};

static const Fields Subscriptions_Fields[] = {
    { QStringLiteral("id"), QStringLiteral("INTEGER PRIMARY KEY") },
    { QStringLiteral("user_id"), QStringLiteral("INTEGER") },
    { QStringLiteral("task"), QStringLiteral("INTEGER") }, LastField
};

static const Fields Users_Fields[] = {
    { QStringLiteral("id"), QStringLiteral("INTEGER PRIMARY KEY") },
    { QStringLiteral("user_id"), QStringLiteral("INTEGER UNIQUE") },
    { QStringLiteral("name"), QStringLiteral("varchar(256)") }, LastField
};

static const Fields *Database_Fields[NumberOfTables] = {
    MetaData_Fields, Installations_Fields, Tasks_Fields, Event_Fields,
    Subscriptions_Fields, Users_Fields
};

const QString DatabaseName = QStringLiteral("charm.kdab.com");
const QString DriverName = QStringLiteral("QSQLITE");

SqLiteStorage::SqLiteStorage()
    : SqlStorage()
    , m_database(QSqlDatabase::addDatabase(DriverName, DatabaseName))
{
    if (!QSqlDatabase::isDriverAvailable(DriverName))
        throw CharmException(QObject::tr("QSQLITE driver not available"));
}

SqLiteStorage::~SqLiteStorage()
{
}

QString SqLiteStorage::lastInsertRowFunction() const
{
    return QStringLiteral("last_insert_rowid");
}

QString SqLiteStorage::description() const
{
    return QObject::tr("local database");
}

bool SqLiteStorage::createDatabaseTables()
{
    Q_ASSERT_X(database().open(), Q_FUNC_INFO,
               "Connection to database must be established first");

    bool error = false;
    // create tables:
    for (int i = 0; i < NumberOfTables; ++i) {
        if (!database().tables().contains(Tables[i])) {
            QString statement;
            QTextStream stream(&statement, QIODevice::WriteOnly);

            stream << "CREATE table  `" << Tables[i] << "` (";
            const Field *field = Database_Fields[i];
            while (field->name != QString::null)
            {
                stream << " `" << field->name << "` "
                       << field->type;
                ++field;
                if (field->name != QString::null) stream << ", ";
            }
            stream << ");";

            QSqlQuery query(database());
            query.prepare(statement);
            if (!runQuery(query))
                error = true;
        }
    }

    error = error
            || !setMetaData(CHARM_DATABASE_VERSION_DESCRIPTOR,
                            QString().setNum(CHARM_DATABASE_VERSION));
    return !error;
}

bool SqLiteStorage::connect(Configuration &configuration)
{   // make sure the database folder exits:
    configuration.failure = true;

    const QFileInfo fileInfo(configuration.localStorageDatabase);   // this is the full path

    // make sure the path exists, file will be created by sqlite
    if (!QDir().mkpath(fileInfo.absolutePath())) {
        configuration.failureMessage = QObject::tr("Cannot make database directory: %1").arg(qt_error_string(
                                                                                                 errno));
        return false;
    }

    if (!QDir(fileInfo.absolutePath()).exists()) {
        configuration.failureMessage
            = QObject::tr("I made a directory, but it is not there. Weird.");
        return false;
    }

    const QDir oldDatabaseDirectory(QDir::homePath() + QDir::separator()
                                    + QStringLiteral(".Charm"));
    if (oldDatabaseDirectory.exists())
        migrateDatabaseDirectory(oldDatabaseDirectory, fileInfo.dir());

    m_database.setHostName(QStringLiteral("localhost"));
    const QString databaseName = fileInfo.absoluteFilePath();
    m_database.setDatabaseName(databaseName);

    bool error = false;

    if (!fileInfo.exists() && !configuration.newDatabase) {
        error = true;
        configuration.failureMessage = QObject::tr(
            "<html><head><meta name=\"qrichtext\" content=\"1\" /></head>"
            "<body><p>The configuration seems to be valid, but the database "
            "file does not exist.</p>"
            "<p>The file will automatically be generated. Please verify "
            "the configuration.</p>"
            "<p>If the configuration is correct, just close the dialog.</p>"
            "</body></html>");
    }

    if (!m_database.open()) {
        configuration.failureMessage = QObject::tr("Could not open SQLite database %1").arg(
            databaseName);
        return false;
    }

    if (!verifyDatabase()) {
        if (!createDatabase(configuration)) {
            configuration.failureMessage = QObject::tr(
                "SqLiteStorage::connect: error creating default database contents");
            return false;
        }
    }

    if (!configuration.newDatabase) {
        const int userid = configuration.user.id();
        const User user = getUser(userid);
        if (!user.isValid())
            return false;

        configuration.user = user;
    }
    // FIXME verify that a database user id has been generated

    if (error)
        return false;

    configuration.failure = false;
    return true;
}

bool SqLiteStorage::migrateDatabaseDirectory(QDir oldDirectory, const QDir &newDirectory) const
{
    if (oldDirectory == newDirectory)
        return true;
    qDebug() << "Application::configure: migrating Charm database directory contents from"
             << oldDirectory.absolutePath() << "to" << newDirectory.absolutePath();

    oldDirectory.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    Q_FOREACH (const QString &entry, oldDirectory.entryList())
        oldDirectory.rename(entry, newDirectory.path() + QDir::separator() + entry);

    QDir oldDirectoryParent(oldDirectory);
    oldDirectoryParent.cdUp();
    return oldDirectoryParent.rmpath(oldDirectory.dirName());
}

bool SqLiteStorage::disconnect()
{
    m_database.removeDatabase(DatabaseName);
    m_database.close();
    return true; // neither of the two methods return a value
}

QSqlDatabase &SqLiteStorage::database()
{
    return m_database;
}

bool SqLiteStorage::createDatabase(Configuration &configuration)
{
    bool success = createDatabaseTables();
    if (!success) return false;

    // add installation id and user id:
    const QString userName = configuration.user.name();
    configuration.user = makeUser(userName);
    if (!configuration.user.isValid()) {
        qDebug() << "SqLiteStorage::createDatabase: cannot store user name";
        return false;
    }

    return true;
}
