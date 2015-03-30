/*
  CommandLine.h

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

#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <QString>

class CommandLine
{
public:
    CommandLine(int argc, char** argv);
    CommandLine(const QString file, const int userId);
    CommandLine(const int userId, const int index);


    enum Mode {
        Mode_None,
        Mode_InitializeDatabase,
        Mode_CheckOrCreateUser,
        Mode_DescribeUsage,
        Mode_PrintVersion,
        Mode_AddTimesheet,
        Mode_RemoveTimesheet,
        Mode_ExportProjectcodes,
        Mode_NumberOfModes
    };

    Mode mode() const;

    QString filename() const;

    QString userComment() const;

    QString exportFilename() const;

    QString userName() const;

    int userid() const;

    int index() const;

    /** Dump command line option reference. */
    static void usage();

private:
    QString m_filename;
    QString m_userComment;
    QString m_userName;
    QString m_exportFilename;
    Mode m_mode;
    int m_index;
    int m_userid;
};

#endif /*COMMANDLINE_H*/
