#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QObject>
#include <QVariant>
#include <QStringList>

#include "Core/CharmExceptions.h"

#include "Database.h"
#include "Exceptions.h"

#include <cstdlib>

Database::Database()
{
}

Database::~Database()
{
}

void Database::checkUserid( int id ) throw (TimesheetProcessorException )
{
	User user = m_storage.getUser( id );
	if ( !user.isValid() ) {
		throw TimesheetProcessorException( "No such user" );
	}
}

User Database::getOrCreateUserByName( QString name ) throw (TimesheetProcessorException )
{
	User user;
	QSqlQuery query( database() );
	const char* statement = "SELECT user_id from Users WHERE name = :user_name;";
	query.prepare( statement );
	query.bindValue( ":user_name", name );
	bool result = query.exec();
	if ( result ) {
		if ( query.next() ) {
			int userIdPosition = query.record().indexOf( "user_id" );
			Q_ASSERT( userIdPosition != -1 );
			int userId = query.value( userIdPosition ).toInt();
			user = m_storage.getUser( userId );
		} else { // user with this name does not exist:
			user = m_storage.makeUser( name ); // that should work
			if( ! user.isValid() ) {
				throw TimesheetProcessorException( "Cannot create the new user" );
			}
		}
	} else {
		throw TimesheetProcessorException( "Cannot execute query for user name" );
	}
	return user;
}

Task Database::getTask( int taskid ) throw (TimesheetProcessorException )
{
	Task task = m_storage.getTask( taskid );
	if ( !task.isValid() ) {
		throw TimesheetProcessorException( QObject::tr( "Invalid task %1 in report" ).arg( taskid ) );
	}
	return task;
}

TaskList Database::getAllTasks() throw(TimesheetProcessorException )
{
	return m_storage.getAllTasks();
}

QSqlDatabase& Database::database()
{
	return m_storage.database();
}
void Database::login() throw (TimesheetProcessorException )
{
	QString name( "timecheater" );
	QString pass( "polkadots" );
	QString host( "127.0.0.1" );
	QString database( "Charm" );
	unsigned int port = 8889;

	// read configuration from the environment:
	char* databaseConfigurationString = getenv( "CHARM_DATABASE_CONFIGURATION" );
	if ( databaseConfigurationString != 0 ) {
		// the string is supposed to be of the format "hostname;port;username;password"
		// of port is 0; the default port is used
		QStringList elements = QString::fromLocal8Bit( databaseConfigurationString ).split( ';' );
		if ( elements.count() != 4 ) {
			throw TimesheetProcessorException( QObject::tr( "Bad database configuration string format" ) );
		} else {
			bool ok;
			host = elements.at( 0 );
			port = elements.at( 1 ).toInt( &ok );
			name = elements.at( 2 );
			pass = elements.at( 3 );
			if ( ok != true || port < 0 ) {
				throw TimesheetProcessorException( QObject::tr(
						"The port must be a non-negative integer number in the database configuration string format" ) );
			}
		}
	}
	m_storage.database().setHostName( host );
	m_storage.database().setDatabaseName( database );
	m_storage.database().setUserName( name );
	m_storage.database().setPassword( pass );
	if ( port != 0 ) {
		m_storage.database().setPort( port );
	}
	bool ok = m_storage.database().open();
	if ( !ok ) {
		QSqlError error = m_storage.database().lastError();

		QString msg = QObject::tr( "Cannot connect to database %1 on host %2, database said "
			"\"%3\", driver said \"%4\"" ) .arg( database ) .arg( host ) .arg( error.driverText() ) .arg( error.databaseText() );
		throw TimesheetProcessorException( msg );
	}
}

void Database::initializeDatabase() throw (TimesheetProcessorException )
{
	try {
		QStringList tables = m_storage.database().tables();
		if ( !tables.empty() ) {
			throw TimesheetProcessorException( "The database is not empty. Only "
				"empty databases can be automatically initialized." );
		}
		if ( !m_storage.createDatabaseTables() ) {
			throw TimesheetProcessorException( "Cannot create database contents, please double-check permissions." );
		}
	} catch ( UnsupportedDatabaseVersionException& e ) {
		throw TimesheetProcessorException( e.what() );
	}
}

void Database::addEvent( const Event& event )
{
	Event newEvent = m_storage.makeEvent();
	int id = newEvent.id();
	newEvent = event;
	newEvent.setId( id );
	if ( !m_storage.modifyEvent( newEvent ) ) {
		throw TimesheetProcessorException( "Cannot add event" );
	}
}

void Database::deleteEventsForReport( int userid, int index )
{
	// delete the time sheet: pretty straightforward
	QString statement = QString::fromLocal8Bit( "DELETE FROM Events WHERE report_id = :index and user_id = :userid" );
	QSqlQuery query( m_storage.database() );
	query.prepare( statement );
	query.bindValue( ":index", index );
	query.bindValue( ":userid", userid );
	bool result = query.exec();
	if ( !result ) {
		throw TimesheetProcessorException( "Failed to delete report" );
	}
}
