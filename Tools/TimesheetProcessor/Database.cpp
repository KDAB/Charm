#include <QSqlDatabase>
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
	QString host("localhost");
	QString database("Charm");
	
	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	db.setHostName( host );
	db.setDatabaseName( database);
	db.setUserName(name);
	db.setPassword(pass);
	bool ok = db.open();
	if (!ok) {
		QString msg = QObject::tr("Cannot connect to database %1 on host %2")
			.arg(database)
			.arg(host);
		throw new TimesheetProcessorException( msg);
	}
}
