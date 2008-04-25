#include <iostream>

#include <QtDebug>
#include <QFile>
#include <QObject>
#include <QDomDocument>
#include <QSqlDatabase>

#include "Core/Event.h"

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
}

void addTimesheet(const CommandLine& cmd)
{
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
	QDomElement charmReportElement = doc.firstChildElement("charmreport");
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
		catch(XmlSerializationException e )
		{
			QString msg = QObject::tr("Syntax error in file %1.").arg(cmd.filename());
			throw TimesheetProcessorException( msg);
		}
	}
	// FIXME maybe verify the project codes, they are also part of the report
	// 2) log into database
	Database database;
	database.login();

	qDebug() << "TimesheetProcessor: adding timesheet" << cmd.index() << "for user" << cmd.userid();

	// 3) map user id to database user id, installation id dito
	// ...
	// 4) add information to the database
	// ...

	// we are connected
	// check for the user id
	// ...
	// FIXME maybe check project codes?
	// ...
	// FIXME 
}

void removeTimesheet(const CommandLine& cmd)
{
	// delete the time sheet: pretty straightforward

	// done
}
