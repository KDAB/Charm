/*
  MySqlStorage.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2008-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>
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

#include "MySqlStorage.h"
#include "CharmConstants.h"

#include <QStringList>
#include <QSqlQuery>
#include <QProcess>

// DATABASE STRUCTURE DEFINITION FOR MYSQL
static const QString Tables[] =
{ "MetaData", "Installations", "Tasks", "Events", "Subscriptions", "Users" };

static const int NumberOfTables = sizeof Tables / sizeof Tables[0];

struct Field
{
        QString name;
        QString type;
};

typedef Field Fields;
const Field LastField =
{ QString::null, QString::null};

static const Fields MetaData_Fields[] =
{
{ "id", "INTEGER AUTO_INCREMENT PRIMARY KEY" },
{ "key", "VARCHAR( 128 ) NOT NULL" },
{ "value", "VARCHAR( 128 )" }, LastField };

static const Fields Installations_Fields[] =
{
{ "id", "INTEGER AUTO_INCREMENT PRIMARY KEY" },
{ "inst_id", "INTEGER" },
{ "user_id", "INTEGER" },
{ "name", "varchar(256)" }, LastField };

static const Fields Tasks_Fields[] =
{
{ "id", "INTEGER AUTO_INCREMENT PRIMARY KEY" },
{ "task_id", "INTEGER UNIQUE" },
{ "parent", "INTEGER" },
{ "validfrom", "timestamp" },
{ "validuntil", "timestamp" },
{ "trackable", "INTEGER" },
{ "name", "varchar(256)" }, LastField };

static const Fields Event_Fields[] =
{
{ "id", "INTEGER AUTO_INCREMENT PRIMARY KEY" },
{ "user_id", "INTEGER" },
{ "event_id", "INTEGER" },
{ "installation_id", "INTEGER" },
{ "report_id", "INTEGER NULL" },
{ "task", "INTEGER" },
{ "comment", "varchar(256)" },
{ "start", "timestamp" },
{ "end", "timestamp" }, LastField };

static const Fields Subscriptions_Fields[] =
{
{ "id", "INTEGER AUTO_INCREMENT PRIMARY KEY" },
{ "user_id", "INTEGER" },
{ "task", "INTEGER" }, LastField };

static const Fields Users_Fields[] =
{
{ "id", "INTEGER AUTO_INCREMENT PRIMARY KEY" },
{ "user_id", "INTEGER UNIQUE" },
{ "name", "varchar(256)" }, LastField };

static const Fields* Database_Fields[NumberOfTables] =
{ MetaData_Fields, Installations_Fields, Tasks_Fields, Event_Fields,
                Subscriptions_Fields, Users_Fields };

const char DatabaseName[] = "mysql.charm.kdab.com";

MySqlStorage::MySqlStorage() :
        SqlStorage(), m_database(QSqlDatabase::addDatabase("QMYSQL", DatabaseName))
{
}

MySqlStorage::~MySqlStorage()
{
}

bool MySqlStorage::createDatabaseTables()
{
        Q_ASSERT_X(database().open(), Q_FUNC_INFO,
                        "Connection to database must be established first");

        bool error = false;
        // create tables:
        for (int i = 0; i < NumberOfTables; ++i)
        {
                if (!database().tables().contains(Tables[i]))
                {
                        QString statement;
                        QTextStream stream(&statement, QIODevice::WriteOnly);

                        stream << "CREATE table  `" << Tables[i] << "` (";
                        const Field* field = Database_Fields[i];
                        while (field->name != QString::null )
                        {
                                stream << " `" << field->name << "` "
                                << field->type;
                                ++field;
                                if ( field->name != QString::null ) stream << ", ";
                        }
                        stream << ");";

                        QSqlQuery query( database() );
                        qDebug() << statement;
                        query.prepare( statement );
                        if ( ! runQuery( query ) )
                        {
                                error = true;
                        }
                }
        }

        error = error || ! setMetaData(CHARM_DATABASE_VERSION_DESCRIPTOR, QString().setNum( CHARM_DATABASE_VERSION) );
        return ! error;
}

QString MySqlStorage::lastInsertRowFunction() const
{
        return QString::fromLocal8Bit("last_insert_id");
}

QSqlDatabase& MySqlStorage::database()
{
        return m_database;
}

QString MySqlStorage::description() const
{
        return QObject::tr("Remote MySql Database");
}

bool MySqlStorage::connect(Configuration&)
{
        return false; // not implemented, needs the right information in Configuration
}

bool MySqlStorage::disconnect()
{
        return false; // not implemented
}

int MySqlStorage::installationId() const
{
        return -1; // not implemented
}

bool MySqlStorage::createDatabase(Configuration& conf)
{
        return createDatabaseTables();
}

MySqlStorage::Parameters MySqlStorage::parseParameterEnvironmentVariable()
{
    // read configuration from the environment:
    const QByteArray databaseConfigurationString = qgetenv( "CHARM_DATABASE_CONFIGURATION" );

    if ( ! databaseConfigurationString.isEmpty() ) {
        Parameters p;
        // the string is supposed to be of the format "hostname;port;username;password"
        // if port is 0 or empty, the default port is used
        QStringList elements = QString::fromLocal8Bit( databaseConfigurationString ).split( ';' );
        if ( elements.count() != 4 ) {
            throw ParseError( QObject::tr( "Bad database configuration string format" ) );
        } else {
            p.host = elements.at( 0 );
            p.name = elements.at( 2 );
            p.password = elements.at( 3 );
            bool ok;
            if( ! elements.at( 1 ).isEmpty() ) {
                p.port = elements.at( 1 ).toUInt( &ok );
                if ( ok != true ) {
                    throw ParseError( QObject::tr(
                            "The port must be a non-negative integer number in the database configuration string format" ) );
                }
            }
        }
        return p;
    } else {
        throw ParseError( QObject::tr( "Timesheet processor configuration not found (CHARM_DATABASE_CONFIGURATION). Aborting." ) );
    }
}

void MySqlStorage::configure( const Parameters& parameters )
{
    database().setHostName( parameters.host );
    database().setDatabaseName( parameters.database );
    database().setUserName( parameters.name );
    database().setPassword( parameters.password );
    if ( parameters.port != 0 ) {
       database().setPort( parameters.port );
    }
}
