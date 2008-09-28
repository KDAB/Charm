#ifndef XMLSERIALIZATION_H
#define XMLSERIALIZATION_H

#include <QDomDocument>
#include <QString>

namespace XmlSerialization {

    QDomDocument createXmlTemplate( QString docClass );

    QDomElement reportElement( const QDomDocument& doc );

    QDomElement metadataElement( const QDomDocument& doc );

}

#endif
