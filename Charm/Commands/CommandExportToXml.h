#ifndef COMMANDEXPORTTOXML_H
#define COMMANDEXPORTTOXML_H

#include <Core/CharmCommand.h>

class QObject;

class CommandExportToXml : public CharmCommand
{
    Q_OBJECT
public:
    explicit CommandExportToXml( QString filename, QObject* parent );
    ~CommandExportToXml();

    bool prepare();
    bool execute( ControllerInterface* );
    bool finalize();

private:
    bool m_error;
    QString m_filename;
};

#endif
