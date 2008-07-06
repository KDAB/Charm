#include <QtSql>
#include <QFile>
#include <QtDebug>
#include <QSqlDatabase>
#include <QSettings>
#include <QStringList>
#include <QTextStream>
#include <QDateTime>

#include "Task.h"
#include "Event.h"
#include "State.h"
#include "SqlStorage.h"
#include "SqlRaiiTransactor.h"
#include "CharmConstants.h"

// SqlStorage class

SqlStorage::SqlStorage() :
	StorageInterface()
{
}

SqlStorage::~SqlStorage()
{
}

bool SqlStorage::verifyDatabase() throw (UnsupportedDatabaseVersionException )
{
	// if the database is empty, it is not ok :-)
	if( database().tables().isEmpty() )
		return false;
	// check database metadata, throw an exception in case the version does not match:
	int version = 1;
	QString versionString = getMetaData(CHARM_DATABASE_VERSION_DESCRIPTOR);
	if (versionString != QString::null )
	{
		int value;
		bool ok;
		value = versionString.toInt( &ok );
		if( ok)
		{
			version = value;
		}
	}
	if( version != REQUIRED_CHARM_DATABASE_VERSION )
	{
		throw UnsupportedDatabaseVersionException( "Database version is not supported." );
	}
	return true;
}

TaskList SqlStorage::getAllTasks()
{
	TaskList tasks;
	QSqlQuery query(database());
	const char statement[] = "select * from Tasks left join Subscriptions "
		"on Tasks.task_id = Subscriptions.task;";
	query.prepare(statement);

	// FIXME merge record retrieval with getTask:
	if (runQuery(query))
	{
		while (query.next())
		{
			Task task = makeTaskFromRecord( query.record() );
			tasks.append(task);
		}
	}

	return tasks;
}

bool SqlStorage::addTask(const Task& task)
{
	QSqlQuery query(database());
	query.prepare("INSERT into Tasks (task_id, name, parent, validfrom, validuntil) "
		"values ( :task_id, :name, :parent, :validfrom, :validuntil);");
	query.bindValue(":task_id", task.id());
	query.bindValue(":name", task.name());
	query.bindValue(":parent", task.parent());
	query.bindValue(":validfrom", task.validFrom() );
	query.bindValue(":validuntil", task.validUntil() );
	return runQuery(query);
}

Task SqlStorage::getTask(int taskid)
{
	QSqlQuery query(database());
	const char
			statement[] =
					"SELECT * FROM Tasks LEFT JOIN Subscriptions ON Tasks.task_id = Subscriptions.task WHERE task_id = :id;";
	query.prepare(statement);
	query.bindValue(":id", taskid);

	if (runQuery(query) && query.next())
	{
		Task task = makeTaskFromRecord( query.record() );
		return task;
	}
	else
	{
		return Task();
	}
}

bool SqlStorage::modifyTask(const Task& task)
{
	QSqlQuery query(database());
	query.prepare("UPDATE Tasks set name = :name, parent = :parent, "
		"validfrom = :validfrom, validuntil = :validuntil "
		"where task_id = :task_id;");
	query.bindValue(":task_id", task.id());
	query.bindValue(":name", task.name());
	query.bindValue(":parent", task.parent());
	query.bindValue(":validfrom", task.validFrom() );
	query.bindValue(":validuntil", task.validUntil() );
	return runQuery(query);
}

bool SqlStorage::deleteTask(const Task& task)
{
	QSqlQuery query(database());
	query.prepare("DELETE from Tasks where task_id = :task_id;");
	query.bindValue(":task_id", task.id());
	return runQuery(query);
}

bool SqlStorage::deleteAllTasks()
{
	QSqlQuery query(database());
	query.prepare("DELETE from Tasks;");
	return runQuery(query);
}

Event SqlStorage::makeEventFromRecord(const QSqlRecord& record)
{
	Event event;

	int idField = record.indexOf("event_id");
	int instIdField = record.indexOf("installation_id");
	int userIdField = record.indexOf("user_id");
	int reportIdField = record.indexOf("report_id");
	int taskField = record.indexOf("task");
	int commentField = record.indexOf("comment");
	int startField = record.indexOf("start");
	int endField = record.indexOf("end");

	event.setId(record.field( idField ).value().toInt() );
	event.setUserId( record.field( userIdField ).value().toInt() );
	event.setReportId( record.field( reportIdField ).value().toInt() );
	event.setInstallationId( record.field ( instIdField ).value().toInt() );
	event.setTaskId( record.field( taskField ).value().toInt() );
	event.setComment( record.field( commentField ).value().toString() );
	if ( ! record.field( startField ).isNull() )
	{
		event.setStartDateTime
		( record.field( startField ).value().value<QDateTime>() );
	}
	if ( !record.field( endField ).isNull() )
	{
		event.setEndDateTime
		( record.field( endField ).value().value<QDateTime>() );
	}

	return event;
}

EventList SqlStorage::getAllEvents()
{
	EventList events;
	QSqlQuery query(database());
	query.prepare("SELECT * from Events;");
	if (runQuery(query))
	{
		while (query.next())
		{
			events.append(makeEventFromRecord(query.record()));
		}
	}
	return events;
}

Event SqlStorage::makeEvent()
{
	SqlRaiiTransactor transactor(database());
	bool result;
	Event event;

	{ // insert a new record in the database
		const char* statement = "INSERT into Events values "
			"( NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL );";
		QSqlQuery query(database());
		query.prepare(statement);
		result = runQuery(query);
		Q_ASSERT(result); // this has to suceed
	}
	if (result)
	{ // retrieve the AUTOINCREMENT id value of it
		const QString statement = QString::fromLocal8Bit(
				"SELECT id from Events WHERE id = %1();") .arg(
				lastInsertRowFunction());
		QSqlQuery query(database());
		query.prepare(statement);
		result = runQuery(query);
		if (result && query.next())
		{
			int indexField = query.record().indexOf("id");
			event.setId(query.value(indexField).toInt());
			event.setInstallationId(installationId());
			Q_ASSERT(event.id() > 0);
		}
		else
		{
			Q_ASSERT_X(false, "SqlStorage::makeEvent",
					"database implementation error (SELECT)");
		}
	}
	if (result)
	{
		// modify the created record to make sure event_id is unique
		// within the installation:
		const char* statement = "UPDATE Events SET event_id = :event_id, "
			"installation_id = :installation, report_id = :report_id WHERE id = :id;";
		QSqlQuery query(database());
		query.prepare(statement);
		query.bindValue(":event_id", event.id());
		query.bindValue(":installation_id", event.installationId());
		query.bindValue(":report_id", event.reportId());
		query.bindValue(":id", event.id());
		result = runQuery(query);
		Q_ASSERT_X(result, "SqlStorage::makeEvent",
				"database implementation error (UPDATE)");
	}

	if (result)
	{
		transactor.commit();
		Q_ASSERT(event.isValid());
		return event;
	}
	else
	{
		return Event();
	}
}

Event SqlStorage::getEvent(int id)
{
	QSqlQuery query(database());
	const char statement[] = "SELECT * FROM Events WHERE event_id = :id;";
	query.prepare(statement);
	query.bindValue(":id", id);

	if (runQuery(query) && query.next())
	{
		Event event = makeEventFromRecord(query.record());
		// FIXME this is going to fail with multiple installations
		Q_ASSERT(!query.next()); // eventid has to be unique
		Q_ASSERT(event.isValid()); // only valid events in database
		return event;
	}
	else
	{
		return Event();
	}
}
bool SqlStorage::modifyEvent(const Event& event)
{
	QSqlQuery query(database());
	query.prepare("UPDATE Events set task = :task, comment = :comment, "
		"start = :start, end = :end, user_id = :user, report_id = :report "
		"where event_id = :id;");
	query.bindValue(":id", event.id());
	query.bindValue(":user", event.userId());
	query.bindValue(":task", event.taskId());
	query.bindValue(":report", event.reportId());
	query.bindValue(":comment", event.comment());
	query.bindValue(":start", event.startDateTime());
	query.bindValue(":end", event.endDateTime() );

	return runQuery( query );
}

bool SqlStorage::deleteEvent(const Event& event)
{
	QSqlQuery query(database());
	query.prepare("DELETE from Events where event_id = :id;");
	query.bindValue(":id", event.id());

	return runQuery(query);
}

bool SqlStorage::deleteAllEvents()
{
	QSqlQuery query(database());
	query.prepare("DELETE from Events;");
	return runQuery(query);
}

#define MARKER "============================================================"

bool SqlStorage::runQuery(QSqlQuery& query)
{
	static const bool DoChitChat = false;
	if (DoChitChat)
		qDebug() << MARKER << endl << "SqlStorage::runQuery: executing query:"
				<< endl << query.executedQuery();
		bool result = query.exec();
		if ( DoChitChat )
		{
			if ( result )
			{

				qDebug() << "SqlStorage::runQuery: SUCCESS" << endl << MARKER;
			}
			else
			{
				qDebug() << "SqlStorage::runQuery: FAILURE" << endl
				<< "Database says: " << query.lastError().databaseText() << endl
				<< "Driver says:   " << query.lastError().driverText() << endl
				<< MARKER;
			}
		}
		return result;
	}

void SqlStorage::stateChanged(State previous)
{
	Q_UNUSED(previous);
	// atm, SqlStorage does not care about state
	// qDebug() << "SqlStorage::stateChanged: NOT IMPLEMENTED"
}

void SqlStorage::populateDatabase()
{
	// temp
	// qDebug() << "SqlStorage::populateDatabase: filling in canned values";

	// simulated startup behaviour:
	TaskList tasks;

	// read the project codes and make plain list:
	// FIXME this is just for development:
	QFile projectsFile("projectcodes.txt");
	if (projectsFile.open(QIODevice::ReadOnly))
	{
		QTextStream stream(&projectsFile);
		QString line;
		while (!(line = stream.readLine()).isNull())
		{
			// weed out comment lines:
			// (everything that does not start with a number)
			if (line.length() < 4 ) continue;
			QString idText = line.left( 4 );
			QString text = line.right( line.length() - 4 );
			bool isNumber;
			int id = idText.toInt( &isNumber );
			if ( !isNumber ) continue;
			text = text.trimmed();
			// weed out categories (1000-1100 Stuff)
			if ( text.at( 0 ) == '-' ) continue;
			// now we can make a task with id as the number and text
			// as the description:
			Task task;
			task.setId( id );
			task.setName( text );
			tasks.append( task );
		}
		qDebug() << "SqlStorage::populateDatabase: imported" << tasks.size()
		<< "tasks from" << projectsFile.fileName();
		// now heuristically determine the task relationships:
		for ( int i = 0; i < tasks.size(); ++i )
		{
			int possibleParent = tasks[i].id();
			int divisor = 10;
			while ( possibleParent> divisor )
			{

				possibleParent = ( possibleParent / divisor ) * divisor;
				divisor *= 10;
				if ( possibleParent == tasks[i].id() ) continue;
				//                 qDebug() << "SqlStorage::populateDatabase: task"
				//                          << tasks[i].id() << "?" << possibleParent;
				for ( int j = 0; j < tasks.size(); ++j )
				{
					if ( tasks[j].id() == possibleParent )
					{
						tasks[i].setParent( tasks[j].id() );
						//                         qDebug() << "SqlStorage::populateDatabase: setting"
						//                                  << tasks[j].name() << "as parent of"
						//                                  << tasks[i].name();
					}
				}
			}
		}
	}
	else
	{
		qDebug() << "SqlStorage::populateDatabase: not importing tasks";
	}

	SqlRaiiTransactor transactor( database() );
	Q_ASSERT( transactor.isActive() ); // sqlite supports transactions
	for ( int i = 0; i < tasks.size(); ++i )
	{
		if ( ! addTask( tasks[i] ) )
		{
			Q_ASSERT_X( false, "SqlStorage::populateDatabase",
			"Cannot add to database" );
		}
	}
	transactor.commit();
}

User SqlStorage::getUser(int userid)
{
	User user;

	QSqlQuery query(database());
	const char* statement = "SELECT * from Users WHERE user_id = :user_id;";
	query.prepare(statement);
	query.bindValue(":user_id", userid);

	if (runQuery(query))
	{
		if (query.next())
		{
			int userIdPosition = query.record().indexOf("user_id");
			int namePosition = query.record().indexOf("name");
			Q_ASSERT(userIdPosition != -1 && namePosition != -1);

			user.setId(query.value(userIdPosition).toInt());
			user.setName(query.value(namePosition).toString());
			Q_ASSERT(user.isValid());
		}
		else
		{
			qDebug() << "SqlStorage::getUser: no user with id" << userid;
		}
	}

	return user;
}

User SqlStorage::makeUser(const QString& name)
{
	SqlRaiiTransactor transactor(database());
	bool result;
	User user;
	user.setName(name);

	{ // create a new record:
		QSqlQuery query(database());
		const char* statement = "INSERT into Users VALUES (NULL, NULL, :name);";

		query.prepare(statement);
		query.bindValue(":name", user.name());

		result = runQuery(query);
		if (!result)
		{
			qDebug() << "SqlStorage::makeUser: FAILED to create new user";
			return user;
		}
	}
	if (result)
	{ // find it and determine key:
		QSqlQuery query(database());

		const QString statement = QString::fromLocal8Bit(
				"SELECT id from Users WHERE id = %1();") .arg(
				lastInsertRowFunction());
		query.prepare(statement);

		result = runQuery(query);
		if (result && query.next())
		{
			int idField = query.record().indexOf("id");
			user.setId(query.value(idField).toInt());
			Q_ASSERT(user.id() != 0);
		}
		else
		{
			qDebug()
					<< "SqlStorage::makeUser: FAILED to find newly created user";
			return user;
		}
	}
	if (result)
	{ // make a unique user id:
		QSqlQuery query(database());
		const char* statement =
				"UPDATE Users SET user_id = :id WHERE id = :idx;";
		query.prepare(statement);
		query.bindValue(":id", user.id());
		query.bindValue(":idx", user.id());
		result = runQuery(query);
		if (!result)
		{
			// all is well
			user.setId(0); // make invalid
		}
	}

	if (result)
		transactor.commit();

	return user;
}

bool SqlStorage::modifyUser(const User& user)
{
	QSqlQuery query(database());
	const char statement[] =
			"UPDATE Users SET name = :name WHERE user_id = :id;";
	query.prepare(statement);
	query.bindValue(":name", user.name());
	query.bindValue(":id", user.id());

	return runQuery(query);
}

bool SqlStorage::deleteUser(const User& user)
{
	QSqlQuery query(database());
	const char statement[] = "DELETE from Users WHERE user_id = :id;";
	query.prepare(statement);
	query.bindValue(":id", user.id());
	return runQuery(query);
}

bool SqlStorage::addSubscription(User user, Task task)
{
	Task dbTask = getTask(task.id());

	if (!dbTask.isValid() || dbTask.isValid() && !dbTask.subscribed())
	{
		QSqlQuery query(database());
		const char statement[] =
				"INSERT into Subscriptions VALUES (NULL, :user_id, :task);";
		query.prepare(statement);
		query.bindValue(":user_id", user.id());
		query.bindValue(":task", task.id());
		return runQuery(query);
	}
	else
	{
		return true;
	}
}

bool SqlStorage::deleteSubscription(User user, Task task)
{
	QSqlQuery query(database());
	const char statement[] =
			"DELETE from Subscriptions WHERE user_id = :user_id AND task = :task;";
	query.prepare(statement);
	query.bindValue(":user_id", user.id());
	query.bindValue(":task", task.id());
	return runQuery(query);
}

Installation SqlStorage::createInstallation(const QString& name)
{
	SqlRaiiTransactor transactor(database());
	bool result;

	Installation installation;
	{ // insert a new record in the database
		const char* statement =
				"INSERT into Installations values ( NULL, NULL, NULL, :name );";
		QSqlQuery query(database());
		query.prepare(statement);
		query.bindValue(":name", name);
		result = runQuery(query);
		Q_ASSERT(result);
		Q_UNUSED(result); // this has to succeed
	}
	if (result)
	{
		// retrieve the AUTOINCREMENT id value of it
		const QString statement = QString::fromLocal8Bit(
				"SELECT * from Installations WHERE id = %1();") .arg(
				lastInsertRowFunction());
		QSqlQuery query(database());
		query.prepare(statement);
		result = runQuery(query);
		if (result && query.next())
		{
			int indexField = query.record().indexOf("id");
			int nameField = query.record().indexOf("name");
			installation.setId(query.value(indexField).toInt());
			installation.setName(query.value(nameField).toString());
			Q_ASSERT(installation.id() > 0);
		}
		else
		{
			Q_ASSERT_X(false, "SqlStorage::makeInstallation",
					"database implementation error (SELECT)");
		}
	}
	if (result)
	{
		// modify the created record to make sure event_id is unique
		// within the installation:
		const char* statement =
				"UPDATE Installations SET inst_id = :inst_id WHERE id = :id;";
		QSqlQuery query(database());
		query.prepare(statement);
		query.bindValue(":inst_id", installation.id());
		query.bindValue(":id", installation.id());
		result = runQuery(query);
		Q_ASSERT_X(result, "SqlStorage::makeInstallation",
				"database implementation error (UPDATE)");
		Q_UNUSED(result);
	}

	if (result)
		transactor.commit();

	return installation;
}

Installation SqlStorage::getInstallation(int installationId)
{
	QSqlQuery query(database());
	const char statement[] = "SELECT * FROM Installations WHERE inst_id = :id;";
	query.prepare(statement);
	query.bindValue(":id", installationId);

	if (runQuery(query) && query.next())
	{
		Installation installation;
		int idField = query.record().indexOf("inst_id");
		int nameField = query.record().indexOf("name");
		int userIdField = query.record().indexOf("user_id");
		installation.setId(query.value(idField).toInt());
		installation.setName(query.value(nameField).toString());
		installation.setUserId(query.value(userIdField).toInt());
		Q_ASSERT(installation.isValid());
		return installation;
	}
	else
	{
		return Installation();
	}
}

bool SqlStorage::modifyInstallation(const Installation& installation)
{
	QSqlQuery query(database());
	const char statement[] =
			"UPDATE Installations SET name = :name, user_id = :user WHERE inst_id = :id;";
	query.prepare(statement);
	query.bindValue(":name", installation.name());
	query.bindValue(":user", installation.userId());
	query.bindValue(":id", installation.id());

	return runQuery(query);
}

bool SqlStorage::deleteInstallation(const Installation& installation)
{
	QSqlQuery query(database());
	const char statement[] = "DELETE from Installations WHERE inst_id = :id;";
	query.prepare(statement);
	query.bindValue(":id", installation.id());
	return runQuery(query);
}

bool SqlStorage::setMetaData(const QString& key, const QString& value)
{
	SqlRaiiTransactor transactor(database());
	// find out if the key is in the database:
	bool result;
	{
		QSqlQuery query(database());
		const char statement[] =
				"SELECT * FROM MetaData WHERE MetaData.key = :key;";
		query.prepare(statement);
		query.bindValue(":key", key);
		if (runQuery(query) && query.next())
		{
			result = true;
		}
		else
		{
			result = false;
		}
	}

	if (result)
	{ // key exists, let's update:
		QSqlQuery query(database());
		const char statement[] =
				"UPDATE MetaData SET value = :value WHERE key = :key;";
		query.prepare(statement);
		query.bindValue(":value", value);
		query.bindValue(":key", key);

		if (runQuery(query))
		{
			transactor.commit();
			return true;
		}
	}
	else
	{
		// key does not exist, let's insert:
		QSqlQuery query(database());
		const char statement[] =
				"INSERT INTO MetaData VALUES ( NULL, :key, :value );";
		query.prepare(statement);
		query.bindValue(":key", key);
		query.bindValue(":value", value);

		if (runQuery(query))
		{
			transactor.commit();
			return true;
		}
		else
		{
			return false;
		}
	}

	return result;
}

QString SqlStorage::getMetaData(const QString& key)
{
	QSqlQuery query(database());
	const char statement[] = "SELECT * FROM MetaData WHERE key = :key;";
	query.prepare(statement);
	query.bindValue(":key", key);

	if (runQuery(query) && query.next())
	{
		int valueField = query.record().indexOf("value");
		return query.value(valueField).toString();
	}
	else
	{
		return QString::null;
	}
}

Task SqlStorage::makeTaskFromRecord( const QSqlRecord& record )
{
	Task task;
	int idField = record.indexOf("task_id");
	int nameField = record.indexOf("name");
	int parentField = record.indexOf("parent");
	int useridField = record.indexOf("user_id");
	int validfromField = record.indexOf("validfrom");
	int validuntilField = record.indexOf("validuntil");

	task.setId(record.field(idField).value().toInt());
	task.setName(record.field(nameField).value().toString());
	task.setParent(record.field(parentField).value().toInt());
	task.setSubscribed(!record.field(useridField).value().toString().isEmpty());
	QString from = record.field(validfromField).value().toString();
	if ( ! from.isEmpty() )
	{
		task.setValidFrom
		( record.field(validfromField).value().value<QDateTime>() );
	}
	QString until = record.field(validuntilField).value().toString();
	if ( ! until.isEmpty() )
	{
		task.setValidUntil
		( record.field( validuntilField ).value().value<QDateTime>() );
	}
	return task;
}
