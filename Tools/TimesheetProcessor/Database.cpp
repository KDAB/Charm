#include <QSqlDatabase>
#include <QSqlError>
#include <QObject>
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

void Database::login() throw ( TimesheetProcessorException )
{
	QString name("timecheater");
	QString pass("polkadots");
	QString host("127.0.0.1");
	QString database("Charm");
	const int port = 8889;
	
	m_storage.database().setHostName( host );
	m_storage.database().setDatabaseName( database);
	m_storage.database().setUserName(name);
	m_storage.database().setPassword(pass);
	m_storage.database().setPort(port);
	bool ok = m_storage.database().open();
	if (!ok) {
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

void Database::initializeDatabase() throw ( TimesheetProcessorException )
{
	try {
		QStringList tables = m_storage.database().tables();
		if( ! tables.empty() ) {
			throw TimesheetProcessorException( "The database is not empty. Only empty databases can be automatically initialized.");
		}
		m_storage.SqlStorage::createDatabase();
	} catch ( UnsupportedDatabaseVersionException& e ) {
		throw TimesheetProcessorException( e.what() );
	}
}

