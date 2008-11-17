/* This program adds or removes timesheet XML files to and from an SQL
 * database.
 */
#include <iostream>

#include <QObject>

#include "CommandLine.h"
#include "Exceptions.h"
#include "Operations.h"

int main(int argc, char** argv)
{
    using namespace std;

    try
    {
        CommandLine cmd(argc, argv);

        switch (cmd.mode())
        {
        case CommandLine::Mode_InitializeDatabase:
            initializeDatabase(cmd);
            break;
        case CommandLine::Mode_CheckOrCreateUser:
            checkOrCreateUser(cmd);
            break;
        case CommandLine::Mode_AddTimesheet:
            addTimesheet(cmd);
            break;
        case CommandLine::Mode_RemoveTimesheet:
            removeTimesheet(cmd);
            break;
        case CommandLine::Mode_ExportProjectcodes:
            exportProjectcodes( cmd );
            break;
        case CommandLine::Mode_DescribeUsage:
        default:
            throw UsageException();
            break;
        }
    } catch (UsageException& e) {
        cerr << e.what() << endl;
        CommandLine::usage();
    } catch (TimesheetProcessorException& e) {
        cerr << e.what() << endl;
        CommandLine::usage();
        return 1;
    }

    return 0;
}
