/*
  main.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2008-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* This program adds or removes timesheet XML files to and from an SQL
 * database.
 */
#include <iostream>

#include <QObject>

#include "CommandLine.h"
#include "Exceptions.h"
#include "Operations.h"
#include "CharmCMake.h"

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
        case CommandLine::Mode_PrintVersion:
            cout << CHARM_VERSION << endl;
            break;
        case CommandLine::Mode_DescribeUsage:
        default:
                CommandLine::usage();
                return 0;
        }
    } catch (const UsageException& e) {
        cerr << e.what() << endl;
        CommandLine::usage();
        return 1;
    } catch (const TimesheetProcessorException& e) {
        cerr << e.what() << endl;
        CommandLine::usage();
        return 1;
    }

    return 0;
}
