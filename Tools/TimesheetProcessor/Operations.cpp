#include <iostream>

#include <QtDebug>
#include <QFile>
#include <QObject>
#include <QDomDocument>
#include <QSqlDatabase>

#include "Core/User.h"
#include "Core/Event.h"
#include "Core/SqlRaiiTransactor.h"

#include "CommandLine.h"
#include "Operations.h"
#include "Exceptions.h"
#include "Database.h"

void initializeDatabase(const CommandLine& cmd)
{
	using namespace std;

	cout << "Initializing database." << endl;

	Database database;
	database.login();
	cout << "Logged in." << endl;

	database.initializeDatabase();

	cout << "Database initialized." << endl;
}

void checkOrCreateUser(const CommandLine& cmd)
{
	using namespace std;
	Database database;
	database.login();
	User user = database.getOrCreateUserByName( cmd.userName( ));
	cout << user.id() << endl;
}

void addTimesheet(const CommandLine& cmd)
{
	using namespace std;
	cout << "Adding report " << cmd.index() << " for user " << cmd.userid()
			<< endl;

	// load the time sheet:
	QFile file(cmd.filename() );
	if ( !file.exists() )
	{
		throw TimesheetProcessorException( QObject::tr("File %1 does not exist.").arg(cmd.filename() ));
	}
	// load the XML into a DOM tree:
	if (!file.open(QIODevice::ReadOnly))
	{
		QString msg = QObject::tr("Cannot open file %1 for reading.").arg(cmd.filename());
		throw TimesheetProcessorException( msg);
	}
	QDomDocument doc("timesheet");
	if (!doc.setContent(&file))
	{
		QString msg = QObject::tr("Cannot read file %1.").arg(cmd.filename());
		throw TimesheetProcessorException( msg);
	}
	// add it to the database:
	// 1) make a list of all the events:
	EventList events;
	int totalSeconds = 0;
	QDomElement charmReportElement = doc.firstChildElement("charmreport");
	QDomElement metadataElement = charmReportElement.firstChildElement("metadata");
	QDomElement yearElement = metadataElement.firstChildElement("year");
	QString year = yearElement.text().simplified();
	QDomElement weekElement = metadataElement.firstChildElement("serial-number");
	QString week = weekElement.text().simplified();
	QDomElement reportElement = charmReportElement.firstChildElement("report");
	QDomElement effortElement = reportElement.firstChildElement("effort");
	if( effortElement.isNull() )
	{
		QString msg = QObject::tr("Invalid structure in file %1.").arg(cmd.filename());
		throw TimesheetProcessorException( msg);
	}
	QDomElement element = effortElement.firstChildElement("event");
	for (; !element.isNull(); element = element.nextSiblingElement("event"))
	{
		try
		{
			Event e = Event::fromXml(element);
			events << e;
			// e.dump();
		}
		catch(XmlSerializationException& e )
		{
			QString msg = QObject::tr("Syntax error in file %1.").arg(cmd.filename());
			throw TimesheetProcessorException( msg);
		}
	}
	// FIXME maybe verify the project codes, they are also part of the report
	// 2) log into database
	Database database;
	database.login();

	// check for the user id
	database.checkUserid(cmd.userid());

	SqlRaiiTransactor transaction( database.database() );
	Q_FOREACH( Event e, events )
	{
		// check for the project code, if this does not throw an exception, the task id exists
		Task task = database.getTask( e.taskId());
		// FIXME check for reporting period for the task, not implemented in the DB
		e.setUserId( cmd.userid() );
		e.setReportId( cmd.index());
		database.addEvent( e );
		totalSeconds += e.duration();
	}
	transaction.commit();
	cout << "Report added" << endl
		<< "total:" << totalSeconds << endl
		<< "year:" << year.toLocal8Bit().constData() << endl
		<< "week:" << week.toLocal8Bit().constData() << endl;
}

void removeTimesheet(const CommandLine& cmd)
{
	using namespace std;
	cout << "Removing report " << cmd.index() << endl;

	Database database;
	database.login();
	database.deleteEventsForReport( cmd.index() );

	cout << "Report " << cmd.index() << " removed" << endl;
}

