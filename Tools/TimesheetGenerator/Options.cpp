extern "C"
{
#include <stdlib.h>
#include <getopt.h>
}

#include <iostream>

#include <QObject>
#include <QFile>

#include "Core/CharmVersion.h"

#include "Exceptions.h"
#include "Options.h"

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

