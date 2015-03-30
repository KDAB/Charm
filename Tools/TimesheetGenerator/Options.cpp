/*
  Options.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2009-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

#include "Options.h"
#include "Exceptions.h"
#include "CharmCMake.h"

#include <QObject>
#include <QFile>

extern "C" {
#include <getopt.h>
}

#include <cstdlib>
#include <iostream>

using namespace TimesheetGenerator;

Options::Options( int argc, char** argv )
{
    opterr = 0;
    int ch;
    while ((ch = getopt(argc, argv, "vhf:d:")) != -1)
    {
        if (ch == '?')
        {
            // unparsable argument
            int option = optopt;
            if (option == 'f')
            {
                throw UsageException(QObject::tr( "Option -f requires a filename argument" ) );
            } else if ( option == 'd' ) {
                throw UsageException(QObject::tr( "Option -d requires a date argument (e.g. 2009-01-01)" ) );
            } else {
                int code = static_cast<int> ( option );
                throw UsageException(QObject::tr("Unknown character %1").arg( code ) );
            }
        }

        switch (ch) {
        case 'f': {
            mFile = QString::fromLocal8Bit(optarg);
            break;
        }
        case 'd': {
            const QString text = QString::fromLocal8Bit( optarg );
            QDate date = QDate::fromString( text, "yyyy-MM-dd" );
            if ( date.isValid() ) {
                mDate = date;
            } else {
                throw UsageException(QObject::tr("Cannot parse date \"%1\"").arg( text ) );
            }
            break;
        }
        case 'h':
            throw UsageException();
        case 'v':
            std::cout << CHARM_VERSION << std::endl;
            exit( 0 );
            break;
        default:
            break;
        }
    }

    if ( mFile.isEmpty() ) {
        throw UsageException(QObject::tr( "No filename specified (-f), aborting." ) );
    }
    if ( ! mDate.isValid() ) {
        throw UsageException( QObject::tr( "No date specified (-d), aborting." ) );
    }

    using namespace std;
    QFile file( mFile );
    if ( ! file.exists() ) {
        throw UsageException(QObject::tr( "Specified file not found, aborting." ) );
    }
}

QString Options::file() const
{
    return mFile;
}

QDate Options::date() const
{
    return mDate;
}
