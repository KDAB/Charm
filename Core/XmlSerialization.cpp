#include <QDateTime>
#include <QFile>

#include "XmlSerialization.h"
#include "CharmExceptions.h"
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

void TaskExport::writeTo( const QString& filename, const TaskList& tasks )
    throw( XmlSerializationException )
{
    QDomDocument document = XmlSerialization::createXmlTemplate( "taskdefinitions" );
    QDomElement metadata = XmlSerialization::metadataElement( document );
    QDomElement report = XmlSerialization::reportElement( document );

    // write tasks
    {
        QDomElement tasksElement = document.createElement( "tasks" );
        report.appendChild( tasksElement );
        Q_FOREACH( const Task& task, tasks ) {
            tasksElement.appendChild( task.toXml( document ) );
        }
    }

    // all done, write to file:
    QFile file( filename );
    if ( file.open( QIODevice::WriteOnly ) ) {
        QTextStream stream( &file );
        document.save( stream, 4 );
    } else {
        throw XmlSerializationException( "" );
    }
}

