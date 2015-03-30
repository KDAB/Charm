/*
  CommandLine.cpp

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

#include "CommandLine.h"
#include "Exceptions.h"

#include <QObject>
#include <QString>

extern "C" {
#include <getopt.h>
}

#include <iostream>

CommandLine::CommandLine(int argc, char** argv) :
        m_mode(Mode_None), m_index(), m_userid()
{
        opterr = 0;
        int ch;
        while ((ch = getopt(argc, argv, "vhza:x:c:ri:u:m:")) != -1)
        {
                if (ch == '?')
                {
                        // unparsable argument
                        int option = optopt;
                        if (option == 'a')
                        {
                                throw UsageException(QObject::tr(
                                                "Option -a requires a filename argument"));
                        }
                        if (option == 'i')
                        {
                                throw UsageException(QObject::tr(
                                                "Option -i requires an index argument"));
                        }
                        if (option == 'x')
                        {
                                throw UsageException(QObject::tr(
                                                "Option -x requires a filename argument"));
                        }
                        if ( option == 'm' )
                        {
                            throw UsageException( QObject::tr( "Option -m requires a user comment argument" ) );
                        }
                        if (isprint(option))
                        {
                                throw UsageException(
                                                QObject::tr("Unknown option %1").arg(option));
                        }
                        else
                        {
                                int code = static_cast<int> (option);
                                throw UsageException(QObject::tr("Unknown character %1").arg(
                                                code));
                        }
                }

                switch (ch)
                {
                case 'a':
                {
                        if (m_mode != Mode_None)
                        {
                                QString msg = QObject::tr(
                                                "Multiple mode selections, please use only one");
                                throw UsageException(msg);
                        }
                        // mode
                        m_filename = QString::fromLocal8Bit(optarg);
                        m_mode = Mode_AddTimesheet;
                        break;
                }
                case 'x':
                {
                        if (m_mode != Mode_None)
                        {
                                QString msg = QObject::tr(
                                                "Multiple mode selections, please use only one");
                                throw UsageException(msg);
                        }
                        // mode
                        m_exportFilename = QString::fromLocal8Bit(optarg);
                        m_mode = Mode_ExportProjectcodes;
                        break;
                }
                case 'c':
                {
                        if (m_mode != Mode_None)
                        {
                                QString msg = QObject::tr(
                                                "Multiple mode selections, please use only one");
                                throw UsageException(msg);
                        }
                        m_userName = QString::fromLocal8Bit(optarg);
                        m_mode = Mode_CheckOrCreateUser;
                        break;

                }
                case 'r': // remove
                        if (m_mode != Mode_None)
                        {
                                QString msg = QObject::tr(
                                                "Multiple mode selections, please use only one");
                                throw UsageException(msg);
                        }
                        m_mode = Mode_RemoveTimesheet;
                        break;
                case 'u': // user id
                {
                        if (m_userid != 0)
                        {
                                QString msg = QObject::tr(
                                                "Multiple user id selections, please use only one");
                                throw UsageException(msg);
                        }

                        QString arg = QString::fromLocal8Bit(optarg);
                        bool ok;
                        m_userid = arg.toInt(&ok);
                        if (!ok || m_userid < 1)
                        {
                                throw UsageException(
                                                QObject::tr(
                                                                "Argument to option -u must be an integer user id larger than zero"));
                        }
                        break;
                }
                case 'i':
                {
                        // index
                        if (m_index != 0)
                        {
                                QString msg = QObject::tr(
                                                "Multiple index selections, please use only one");
                                throw UsageException(msg);
                        }

                        QString arg = QString::fromLocal8Bit(optarg);
                        bool ok;
                        m_index = arg.toInt(&ok);
                        if (!ok || m_index < 1)
                        {
                                throw UsageException(
                                                QObject::tr(
                                                                "Argument to option -i must be an integer index larger than zero"));
                        }
                        break;
                }
                case 'm':
                {
                    if ( ! m_userComment.isEmpty() )
                    {
                        QString msg = QObject::tr( "Multiple user comments specified, please use only one" );
                        throw UsageException( msg );
                    }
                    QString arg = QString::fromLocal8Bit( optarg );
                    m_userComment = arg;
                    break;
                }
                case 'z':
                        // initialize the database
                        m_mode = Mode_InitializeDatabase;
                        break;
                case 'v':
                    m_mode = Mode_PrintVersion;
                    break;
                case 'h':
                default:
                        // help/usage
                        m_mode = Mode_DescribeUsage;
                        //return;
                }
        }

        // check for other command line arguments and yell at the user:
        if (optind < argc)
        {
                QString msg;
                for (int index = optind; index < argc; index++)
                {
                        msg += QObject::tr("Unknown extra argument \"%1\"\n").arg(
                                        argv[index]);
                }
                throw UsageException(msg);
        }

        // final checks:
        QString msg;
        if (m_mode == Mode_None)
        {
                msg += QObject::tr("No mode selected. Use one of -a filename, -r.");
        }
        else if ( m_mode == Mode_RemoveTimesheet)
        {
                if (m_index < 1)
                {
                        msg += QObject::tr("No index specified. -a filename, "
                                " -r require an index specified with -i.");
                }

                if (m_userid < 1)
                {
                        msg += QObject::tr("No userid specified. -a filename, "
                                " -r require a user id specified with -u.");
                }
        } else if ( m_mode == Mode_AddTimesheet ) {
            if ( m_index > 0 ) {
                msg += QObject::tr( "Specifying an index when adding a time sheet is not supported anymore." );
            }
        }

        if (!msg.isEmpty())
        {
                throw UsageException(msg);
        }
}

CommandLine::CommandLine(const QString file, const int userId)
{
    m_filename = file;
    m_userid = userId;
}

CommandLine::CommandLine(const int userId, const int index)
{
    m_userid = userId;
    m_index = index;
}

CommandLine::Mode CommandLine::mode() const
{
        return m_mode;
}

QString CommandLine::userName() const
{
        return m_userName;
}

QString CommandLine::userComment() const
{
    return m_userComment;
}

QString CommandLine::filename() const
{
        return m_filename;
}

QString CommandLine::exportFilename() const
{
    return m_exportFilename;
}

int CommandLine::userid() const
{
        return m_userid;
}

int CommandLine::index() const
{
        return m_index;
}

void CommandLine::usage()
{
        using namespace std;
        cout << "Timesheet Processor, (C) 2008 Mirko Boehm, KDAB" << endl
                        << "Usage: " << endl
                        << "   * TimesheetProzessor -h                                         <-- get help"
                        << endl
                        << "   * TimesheetProzessor -v                                         <-- print version"
                        << endl
                        << "   * TimesheetProzessor -a filename -u userid -m comment  <-- add timesheet from file"
                        << endl
                        << "   * TimesheetProzessor -r -i index -u userid                      <-- remove timesheet at index"
                        << endl
                        << "   * TimesheetProzessor -c username                                <-- create user if user does not exist"
                        << endl
                        << "   * TimesheetProzessor -x filename                                <-- export project codes to XML file"
                        << endl
                        << "   * TimesheetProzessor -z                                         <-- initialize database (careful!)"
                        << endl;
}
