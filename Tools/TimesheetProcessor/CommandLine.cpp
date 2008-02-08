extern "C" {
	#include <getopt.h>
}

#include <iostream>

#include "CommandLine.h"

CommandLine::CommandLine(int argc, char** argv) :
	m_mode(Mode_DescribeUsage)
{
	int ch;
	while ((ch = getopt(argc, argv, "hr:a:f:")) != -1) {
		switch (ch) {
		case 'h':
		case '?':
			m_mode = Mode_DescribeUsage;
			break;
		case 'a': // add
			m_mode = Mode_AddTimesheet;
			break;
		case 'r': // remove
			m_mode = Mode_RemoveTimesheet;
			break;
		case 'f': // filename
			// ...
			break;
		}
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
		<< "Usage: ..." << endl;
}
