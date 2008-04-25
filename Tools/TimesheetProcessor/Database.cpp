#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QObject>
#include <QVariant>
#include <QStringList>

#include "Core/CharmExceptions.h"

#include "Database.h"
#include "Exceptions.h"

Database::Database()
{
}

Database::~Database()
{
}

void Database::checkUserid( int id ) throw (TimesheetProcessorException )
{
	User user = m_storage.getUser(id );
	if( ! user.isValid()  ) {
		throw TimesheetProcessorException( "No such user");
	}
}

Task Database::getTask( int taskid ) throw (TimesheetProcessorException )
{
	Task task = m_storage.getTask( taskid );
	if( ! task.isValid() ) {
		throw TimesheetProcessorException( "Invalid task in report");
	}
	return task;
}


QSqlDatabase& Database::database()
{
	return m_storage.database();
}
void Database::login() throw (TimesheetProcessorException )
{
	QString name("timecheater");
	QString pass("polkadots");
	QString host("127.0.0.1");
	QString database("Charm");
	const int port = 8889;

	m_storage.database().setHostName(host);
	m_storage.database().setDatabaseName(database);
	m_storage.database().setUserName(name);
	m_storage.database().setPassword(pass);
	m_storage.database().setPort(port);
	bool ok = m_storage.database().open();
	if (!ok)
	{
		QSqlError error = m_storage.database().lastError();

		QString msg = QObject::tr("Cannot connect to database %1 on host %2, database said "
				"\"%3\", driver said \"%4\"")
		.arg(database)
		.arg(host)
		.arg(error.driverText())
		.arg(error.databaseText());
		throw TimesheetProcessorException( msg);
	}
}

void Database::initializeDatabase() throw (TimesheetProcessorException )
{
	try
	{
		QStringList tables = m_storage.database().tables();
		if (!tables.empty() )
		{
			throw TimesheetProcessorException( "The database is not empty. Only "
			"empty databases can be automatically initialized.");
		}
		if( ! m_storage.createDatabaseTables() )
		{
			throw TimesheetProcessorException( "Cannot create database contents, please double-check permissions.");
		}
	}
	catch ( UnsupportedDatabaseVersionException& e )
	{
		throw TimesheetProcessorException( e.what() );
	}
}

void Database::addEvent(const Event& event)
{
	Event newEvent = m_storage.makeEvent();
	int id = newEvent.id();
	newEvent = event;
	newEvent.setId(id);
	if (!m_storage.modifyEvent(newEvent))
	{
		throw TimesheetProcessorException("Cannot add event");
	}
}

void Database::deleteEventsForReport(int index)
{
	// delete the time sheet: pretty straightforward
	QString statement = QString::fromLocal8Bit( "DELETE FROM EVENTS WHERE report_id = :index");
	QSqlQuery query( m_storage.database() );
	query.prepare( statement );
	query.bindValue( ":index", index );
	bool result = query.exec();
	if( ! result ) {
		throw TimesheetProcessorException("Failed to delete report");
	}
}
