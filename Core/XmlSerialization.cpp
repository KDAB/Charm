#include <QDateTime>
#include <QFile>

#include "XmlSerialization.h"
#include "CharmExceptions.h"
#include "Configuration.h"
#include "CharmConstants.h"

static QHash<QString,QString> readMetadata( const QDomElement& metadata ) {
    QHash<QString,QString> l;
    const QDomNodeList cs = metadata.childNodes();
    for ( int i = 0; i < cs.count(); ++i ) {
        QDomElement e = cs.at( i ).toElement();
        if ( e.isNull() )
            continue;
        l.insert( e.tagName(), e.text() );
    }
    return l;
}

namespace XmlSerialization {

    QString reportTagName() { 
        return "charmreport";
    }
    
    QString reportTypeAttribute() {
        return "type";
    }

    QDomDocument createXmlTemplate( QString docClass )
    {
        QDomDocument doc( reportTagName() );

        // root element:
        QDomElement root = doc.createElement( reportTagName() );
        root.setAttribute( reportTypeAttribute(), docClass );
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

QString TaskExport::reportType()
{
    return "taskdefinitions";
}

void TaskExport::writeTo( const QString& filename, const TaskList& tasks )
{
    QDomDocument document = XmlSerialization::createXmlTemplate( reportType() );
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
        throw XmlSerializationException( QObject::tr( "Cannot write to file: %1" ).arg( file.errorString() ) );
    }
}

void TaskExport::readFrom( const QString& filename )
{
    // load the time sheet:
    QFile file( filename );
    if ( !file.exists() )
    {
        throw XmlSerializationException( QObject::tr( "File does not exist." ) );
    }
    // load the XML into a DOM tree:
    if (!file.open(QIODevice::ReadOnly))
    {
        throw XmlSerializationException( QObject::tr( "Cannot open file for reading: %1" ).arg( file.errorString() ) );
    }

    return readFrom( &file );
}

void TaskExport::readFrom( QIODevice* device )
{
    QDomDocument document;
    QString errorMessage;
    int errorLine = 0;
    int errorColumn = 0;
    if (!document.setContent(device, &errorMessage, &errorLine, &errorColumn))
    {
        throw XmlSerializationException( QObject::tr( "Invalid XML: [%1:%2] %3" ).arg( QString::number( errorLine ), QString::number( errorColumn ), errorMessage ) );
    }

    // now read and check for the correct report type
    QDomElement rootElement = document.documentElement();
    const QString tagName = rootElement.tagName();
    const QString typeAttribute = rootElement.attribute( XmlSerialization::reportTypeAttribute() );
    if( tagName != XmlSerialization::reportTagName() || typeAttribute != reportType() ) {
        throw XmlSerializationException( QObject::tr( "This file is not a Charm task definition file. Please double-check." ) );
    }

    QDomElement metadata = XmlSerialization::metadataElement( document );
    QDomElement report = XmlSerialization::reportElement( document );

    m_metadata = readMetadata( metadata );

    // from metadata, read the export time stamp:
    m_exportTime = XmlSerialization::creationTime( metadata );
    // from report, read tasks:
    QDomElement tasksElement = report.firstChildElement( Task::taskListTagName() );
    m_tasks = Task::readTasksElement( tasksElement, CHARM_DATABASE_VERSION );
}

const TaskList& TaskExport::tasks() const
{
    return m_tasks;
}

QDateTime TaskExport::exportTime() const
{
    return m_exportTime;
}

QString TaskExport::metadata( const QString& key ) const
{
    return m_metadata.value( key, QString() );
}
