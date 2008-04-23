#include <QSqlDatabase>
#include <QSqlError>
#include <QObject>

#include "Database.h"
#include "Exceptions.h"

Database::Database()
{
}

Database::~Database()
{
}

void Database::login()
{
	QString name("timecheater");
	QString pass("polkadots");
	QString host("127.0.0.1");
	QString database("Charm");
	const int port = 8889;
	
	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	db.setHostName( host );
	db.setDatabaseName( database);
	db.setUserName(name);
	db.setPassword(pass);
	db.setPort(port);
	bool ok = db.open();
	if (!ok) {
		QSqlError error = db.lastError();
		
		QString msg = QObject::tr("Cannot connect to database %1 on host %2, database said "
				"\"%3\", driver said \"%4\"")
			.arg(database)
			.arg(host)
			.arg(error.driverText())
			.arg(error.databaseText());
		throw new TimesheetProcessorException( msg);
	}
}
