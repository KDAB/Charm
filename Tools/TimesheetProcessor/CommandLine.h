#ifndef COMMANDLINE_H_
#define COMMANDLINE_H_

#include <QString>

class CommandLine
{
public:
    CommandLine(int argc, char** argv);

    enum Mode {
        Mode_None,
        Mode_InitializeDatabase,
        Mode_CheckOrCreateUser,
        Mode_DescribeUsage,
        Mode_AddTimesheet,
        Mode_RemoveTimesheet,
        Mode_ExportProjectcodes,
        Mode_NumberOfModes
    };

    Mode mode() const;

    QString filename() const;

    QString exportFilename() const;

    QString userName() const;

    int userid() const;

    int index() const;

    /** Dump command line option reference. */
    static void usage();

private:
    QString m_filename;
    QString m_userName;
    QString m_exportFilename;
    Mode m_mode;
    int m_index;
    int m_userid;
};

#endif /*COMMANDLINE_H_*/
