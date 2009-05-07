extern "C"
{
#include <getopt.h>
}

#include <iostream>

#include <QObject>
#include <QFile>

#include "Exceptions.h"
#include "Options.h"

using namespace TimesheetGenerator;

Options::Options( int argc, char** argv )
{
    opterr = 0;
    int ch;
    while ((ch = getopt(argc, argv, "hf:")) != -1)
    {
        if (ch == '?')
        {
            // unparsable argument
            int option = optopt;
            if (option == 'f')
            {
                throw UsageException(QObject::tr( "Option -f requires a filename argument" ) );
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
        case 'h':
        default:
            break;
        }
    }

    if ( mFile.isEmpty() ) {
        throw UsageException(QObject::tr( "No filename specified (-f), aborting." ) );
    }

    using namespace std;
    cout << "Parsing file \"" << qPrintable( mFile ) << "\"... ";
    QFile file( mFile );
    if ( ! file.exists() ) {
        cout << "not found." << endl;
        throw UsageException(QObject::tr( "Specified file not found, aborting." ) );
    } else {
        cout << "found." << endl;
    }
}

QString Options::file() const
{
    return mFile;
}
