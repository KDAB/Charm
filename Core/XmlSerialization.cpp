#include <QDateTime>
#include <QFile>

#include "XmlSerialization.h"
#include "CharmExceptions.h"
#include "Configuration.h"
#include "CharmConstants.h"

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

    QDateTime creationTime( const QDomElement& metaDataElement )
    {
        QDomElement creationTimeElement = metaDataElement.firstChildElement( "creation-time" );
        if ( ! creationTimeElement.isNull() ) {
            return QDateTime::fromString( creationTimeElement.text(), Qt::ISODate );
        } else {
            return QDateTime();
        }
    }

    QString userName( const QDomElement& metaDataElement )
    {
        QDomElement usernameElement = metaDataElement.firstChildElement( "username" );
        return usernameElement.text();
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
        QDomElement tasksElement = Task::makeTasksElement( document, tasks );
        report.appendChild( tasksElement );
    }

    // all done, write to file:
    QFile file( filename );
    if ( file.open( QIODevice::WriteOnly ) ) {
        QTextStream stream( &file );
        document.save( stream, 4 );
    } else {
        throw XmlSerializationException( "Cannot write to file" );
    }
}

void TaskExport::readFrom( const QString& filename )
{
    // load the time sheet:
    QFile file( filename );
    if ( !file.exists() )
    {
        throw XmlSerializationException( "File does not exist." );
    }
    // load the XML into a DOM tree:
    if (!file.open(QIODevice::ReadOnly))
    {
        throw XmlSerializationException( "Cannot open file for reading." );
    }
    QDomDocument document;
    if (!document.setContent(&file))
    {
        throw XmlSerializationException( "Cannot read file" );
    }

    // now read
    QDomElement metadata = XmlSerialization::metadataElement( document );
    QDomElement report = XmlSerialization::reportElement( document );

    // from metadata, read the export time stamp:
    m_exportTime = XmlSerialization::creationTime( metadata );
    m_userName = XmlSerialization::userName( metadata );
    // from report, read tasks:
    QDomElement tasksElement = report.firstChildElement( Task::taskListTagName() );
    m_tasks = Task::readTasksElement( tasksElement, CHARM_DATABASE_VERSION );
    qDebug() << "XmlSerialization::readFrom: loaded task definitions exported by"
             << m_userName << "as of" << m_exportTime;
}

const TaskList& TaskExport::tasks() const
{
    return m_tasks;
}

QDateTime TaskExport::exportTime() const
{
    return m_exportTime;
}


