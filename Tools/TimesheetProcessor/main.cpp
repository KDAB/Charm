/* This program adds or removes timesheet XML files to and from an SQL
 * database.
 */
#include <iostream>

#include <QObject>

#include "CommandLine.h"
#include "Exceptions.h"

int main(int argc, char** argv)
{
	using namespace std;

	try {
		CommandLine cmd( argc, argv);

		switch( cmd.mode() ) {
			case CommandLine::Mode_AddTimesheet:
				// load the time sheet:
				// add it to the database:
				break;				
			case CommandLine::Mode_RemoveTimesheet:
				// delete the time sheet: pretty straightforward
				// done
				break;
			case CommandLine::Mode_DescribeUsage:
			default:
				throw new UsageException();
			break;
		}
	} catch( UsageException* e ) {
		CommandLine::usage();
	} catch ( TimesheetProcessorException* e ) {
		cout << e->what() << endl;
		return 1;
	}
	
	return 0;
}
