#ifndef XMLSERIALIZATION_H
#define XMLSERIALIZATION_H

#include <QDomDocument>
#include <QString>

#include "Task.h"
#include "CharmExceptions.h"

namespace XmlSerialization {

    QDomDocument createXmlTemplate( QString docClass );

    QDomElement reportElement( const QDomDocument& doc );

    QDomElement metadataElement( const QDomDocument& doc );
}

class TaskExport {
public:
    // the only method that deals with writing:
    static void writeTo( const QString& filename, const TaskList& tasks )
        throw( XmlSerializationException );

    void readFrom( const QString& filename );
    const TaskList& tasks() const;
    QDateTime exportTime() const;
private:
    TaskList m_tasks;
    QDateTime m_exportTime;
};

#endif
