#include <iostream>

#include <QtDebug>
#include <QFile>
#include <QObject>
#include <QDomDocument>
#include <QSqlDatabase>
#include <QVariant>
#include <QSqlQuery>
#include <QSqlRecord>

#include <Core/User.h>
#include <Core/Event.h>
#include <Core/SqlRaiiTransactor.h>
#include <Core/XmlSerialization.h>

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
		try {
			Event e = Event::fromXml(element);
			events << e;
			// e.dump();
		} catch(XmlSerializationException& e ) {
			QString msg = QObject::tr("Syntax error in file %1.").arg(cmd.filename());
			throw TimesheetProcessorException( msg );
		}
	}
	// 2) log into database
	Database database;
	database.login();
        int index = -1;

	// check for the user id
	database.checkUserid(cmd.userid());

	SqlRaiiTransactor transaction( database.database() );
        // add time sheet to time sheets list
        {
            QSqlQuery query( database.database() );
            query.prepare( "INSERT into timesheets VALUES( 0, :filename, NULL, NULL, NULL, NULL, :userid, 0)" );
            query.bindValue( QString::fromAscii( ":filename" ), cmd.filename() );
            // FIXME add original file name?
            query.bindValue( ":userid", cmd.userid() );
            if ( ! query.exec() ) {
                QString msg = QObject::tr( "Error adding time sheet %1.").arg(cmd.filename() );
                throw TimesheetProcessorException( msg );
            }
        }
        // retrieve index
        {
            QSqlQuery query( database.database() );
            if ( ! query.exec( "SELECT id from timesheets WHERE id = last_insert_id()" ) ) {
                QString msg = QObject::tr( "SQL error retrieving index for time sheet %1.").arg(cmd.filename() );
                throw TimesheetProcessorException( msg );
            }

            if ( query.next() ) {
                const int idField = query.record().indexOf( "id" );
                index = query.value( idField ).toInt();
            } else {
                QString msg = QObject::tr( "Error retrieving index for time sheet %1.").arg(cmd.filename() );
                throw TimesheetProcessorException( msg );
            }
        }
        Q_ASSERT( index > 0 );
        // add the events to the database
	Q_FOREACH( Event e, events )
	{
		// check for the project code, if this does not throw an exception, the task id exists
		Task task = database.getTask( e.taskId());
		// FIXME check for reporting period for the task, not implemented in the DB
		e.setUserId( cmd.userid() );
		e.setReportId( index );
		database.addEvent( e );
		totalSeconds += e.duration();
	}
	transaction.commit();
	cout << "Report added" << endl
		<< "total:" << totalSeconds << endl
		<< "year:" << year.toLocal8Bit().constData() << endl
		<< "week:" << week.toLocal8Bit().constData() << endl
                << "index:" << index << endl;
}

void removeTimesheet(const CommandLine& cmd)
{
	using namespace std;
	cout << "Removing report " << cmd.index() << endl;

	Database database;
	database.login();
	SqlRaiiTransactor transaction( database.database() );
	database.deleteEventsForReport( cmd.userid(), cmd.index() );
        {
            QSqlQuery query( database.database() );
            query.prepare( "DELETE from timesheets WHERE id = :index" );
            query.bindValue( QString::fromAscii( ":index" ), cmd.index() );

            if ( ! query.exec() ) {
                QString msg = QObject::tr( "Error removing timesheet %1.").arg(cmd.index() );
                throw TimesheetProcessorException( msg );
            }
        }

	transaction.commit();
	cout << "Report " << cmd.index() << " removed" << endl;
}

void exportProjectcodes( const CommandLine& cmd )
{
    using namespace std;

    cout << "Exporting project codes to " << qPrintable( cmd.exportFilename() ) << endl;

    Database database;
    database.login();

    TaskList tasks = database.getAllTasks();
    try {
        TaskExport::writeTo( cmd.exportFilename(), tasks );
    } catch ( XmlSerializationException& e) {
        throw TimesheetProcessorException( QObject::tr( "Cannot write to file %1" ).arg( cmd.exportFilename() ) );
    }

    cout << "Done, " << tasks.count() << " tasks definitions exported." << endl;
}
