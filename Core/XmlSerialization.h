#ifndef CHARM_XMLSERIALIZATION_H
#define CHARM_XMLSERIALIZATION_H

#include <QDomDocument>
#include <QHash>
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
    void readFrom( QIODevice* device );

    const TaskList& tasks() const;
    QString metadata( const QString& key ) const;
    static QString reportType();

    QDateTime exportTime() const;

private:

    TaskList m_tasks;
    QHash<QString,QString> m_metadata;
    QDateTime m_exportTime;
};

#endif
