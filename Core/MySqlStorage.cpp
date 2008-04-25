/*
 * MySqlStorage.cpp
 *
 *  Created on: Apr 23, 2008
 *      Author: mirko
 */
#include <QSqlDatabase>
#include <QStringList>
#include <QSqlQuery>

#include "MySqlStorage.h"
#include "CharmConstants.h"

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
{ "start", "date" },
{ "end", "date" }, LastField };

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

const char DatabaseName[] = "mysql.charm.kdab.net";

MySqlStorage::MySqlStorage() :
	SqlStorage(), m_database(QSqlDatabase::addDatabase("QMYSQL", DatabaseName))
{
}

MySqlStorage::~MySqlStorage()
{
}

bool MySqlStorage::createDatabaseTables()
{
	Q_ASSERT_X(database().open(), "SqlStorage::createDatabase",
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
	// FIXME temp remove this:
	// populateDatabase();

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
