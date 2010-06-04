#ifndef CHARM_XMLSERIALIZATION_H
#define CHARM_XMLSERIALIZATION_H

#include <QDomDocument>
#include <QString>

#include "Task.h"

namespace XmlSerialization {

    QDomDocument createXmlTemplate( QString docClass );

    QDomElement reportElement( const QDomDocument& doc );

    QDomElement metadataElement( const QDomDocument& doc );

    QDateTime creationTime( const QDomElement& metaDataElement );
    QString userName( const QDomElement& metaDataElement );
}

class TaskExport {
public:
    // the only method that deals with writing:
    static void writeTo( const QString& filename, const TaskList& tasks );
    void readFrom( const QString& filename );
    const TaskList& tasks() const;
    QDateTime exportTime() const;
    static QString reportType();

private:
    TaskList m_tasks;
    QString m_userName;
    QDateTime m_exportTime;
};

#endif
