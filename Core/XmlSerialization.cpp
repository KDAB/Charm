#include <QDateTime>

#include "XmlSerialization.h"
#include "Configuration.h"

namespace XmlSerialization {

    QDomDocument createXmlTemplate( QString docClass )
    {
        QDomDocument doc( "charmreport" );

        // root element:
        QDomElement root = doc.createElement( "charmreport" );
        root.setAttribute( "type", docClass );
        doc.appendChild( root );

        // metadata:
        {
            QDomElement metadata = doc.createElement( "metadata" );
            root.appendChild( metadata );
            QDomElement username = doc.createElement( "username" );
            metadata.appendChild( username );
            QDomText text = doc.createTextNode( Configuration::instance().user.name() );
            username.appendChild( text );
            QDomElement creationTime = doc.createElement( "creation-time" );
            metadata.appendChild( creationTime );
            QDomText time = doc.createTextNode(
                QDateTime::currentDateTime().toUTC().toString( Qt::ISODate ) );
            creationTime.appendChild( time );
            // FIXME installation id and stuff are probably necessary
        }

        QDomElement report = doc.createElement( "report" );
        root.appendChild( report );

        return doc;
    }

    QDomElement reportElement( const QDomDocument& document )
    {
        QDomElement root = document.documentElement();
        return root.firstChildElement( "report" );

    }

    QDomElement metadataElement( const QDomDocument& document )
    {
        QDomElement root = document.documentElement();
        return root.firstChildElement( "metadata" );
    }

}
