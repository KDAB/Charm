#ifndef COMMANDLINE_H_
#define COMMANDLINE_H_

#include <QString>

class CommandLine 
{
public:
	CommandLine(int argc, char** argv);
	
	enum Mode {
		Mode_DescribeUsage,
		Mode_AddTimesheet,
		Mode_RemoveTimesheet,
		Mode_NumberOfModes
	};
	
	Mode mode() const;
	
	QString filename() const;
	
	/** Dump command line option reference. */
	static void usage();
	
private:
	QString m_filename;
	Mode m_mode;
};

#endif /*COMMANDLINE_H_*/
