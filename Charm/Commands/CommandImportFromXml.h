#ifndef COMMANDIMPORTFROMXML_H
#define COMMANDIMPORTFROMXML_H

#include <Core/CharmCommand.h>

class QObject;

class CommandImportFromXml : public CharmCommand
{
    Q_OBJECT
public:
    explicit CommandImportFromXml( QString filename, QObject* parent );
    ~CommandImportFromXml();

    bool prepare();
    bool execute( ControllerInterface* );
    bool finalize();

private:
    bool m_error;
    QString m_filename;
};

#endif
