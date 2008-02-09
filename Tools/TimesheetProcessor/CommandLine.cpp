extern "C" {
#include <getopt.h>
}

#include <iostream>

#include <QObject>
#include <QString>

#include "CommandLine.h"
#include "Exceptions.h"

CommandLine::CommandLine(int argc, char** argv) :
	m_mode(Mode_None), m_index()
{
	opterr = 0;
	char ch;
	while ((ch = getopt(argc, argv, "ha:ri:")) != -1) {
		if (ch == '?') {
			// unparsable argument
			char option = optopt;
			if (option == 'a') {
				throw new UsageException
				(QObject::tr("Option -a requires a filename argument"));
			}
			if (option == 'i') {
				throw new UsageException
				(QObject::tr("Option -i requires an index argument"));
			}
			if (isprint(option)) {
				throw new UsageException
				(QObject::tr("Unknown option %1").arg(option));
			} else {
				int code = static_cast<int>(option);
				throw new UsageException
				(QObject::tr("Unknown character %1").arg(code));
			}
		}

		switch (ch) {
		case 'a': {
			if (m_mode != Mode_None) {
				QString
						msg =
								QObject::tr("Multiple mode selections, please use only one");
				throw new UsageException(msg);
			}
			// mode
			m_filename = QString::fromLocal8Bit(optarg);
			m_mode = Mode_AddTimesheet;
			break;
		}
		case 'r': // remove
			if (m_mode != Mode_None) {
				QString
						msg =
								QObject::tr("Multiple mode selections, please use only one");
				throw new UsageException(msg);
			}
			m_mode = Mode_RemoveTimesheet;
			break;
		case 'i': {
			// index
			if (m_index != 0) {
				QString
						msg =
								QObject::tr("Multiple index selections, please use only one");
				throw new UsageException(msg);
			}

			QString arg = QString::fromLocal8Bit(optarg);
			bool ok;
			m_index = arg.toInt(&ok);
			if ( !ok || m_index < 1) {
				throw new UsageException
				(QObject::tr("Argument to option -i must be an integer index larger than zero"));
			}
			break;
		}
		case 'h':
		default:
			// help/usage
			m_mode = Mode_DescribeUsage;
			//return;
		}
	}

	// check for other command line arguments and yell at the user:
	if (optind < argc) {
		QString msg;
		for (int index = optind; index < argc; index++) {
			msg += QObject::tr("Unknown extra argument \"%1\"\n").arg(argv[index]);
		}
		throw new UsageException(msg);
	}

	// final checks:
	QString msg;
	if (m_mode == Mode_None) {
		msg += QObject::tr("No mode selected. Use one of -a filename, -r.");
	} else if (m_mode == Mode_AddTimesheet || m_mode == Mode_RemoveTimesheet) {
		if (m_index < 1) {
			msg += QObject::tr("No index specified. -a filename, "
				" -r require an index specified with -i.");
		}
	} else {
		// all is well
	}
	if ( !msg.isEmpty() ) {
		throw new UsageException(msg);
	}
}

CommandLine::Mode CommandLine::mode() const
{
	return m_mode;
}

QString CommandLine::filename() const
{
	return m_filename;
}

void CommandLine::usage()
{
	using namespace std;
	cout << "Timesheet Processor, (C) 2008 Mirko Boehm, KDAB" << endl
			<< "Usage: " << endl
			<< "   * TimesheetProzessor -h                              get help"
			<< endl
			<< "   * TimesheetProzessor -a filename -i index            add timesheet from file with index"
			<< endl
			<< "   * TimesheetProzessor -r -i index                     remove timesheet at index"
			<< endl;
}
