#include <iostream>

#include <QDomDocument>
#include <QDomElement>
#include <QObject>
#include <QFile>

#include "Core/Event.h"
#include "Core/CharmExceptions.h"
#include <Core/XmlSerialization.h>

#include "Exceptions.h"
#include "Options.h"

int main( int argc, char** argv ) {
    using namespace std;

    try {
        using namespace TimesheetGenerator;
        cout << "Timesheet Generator, (C) 2009 Mirko Boehm, KDAB" << endl;
        Options options( argc, argv );

        // create a QDomDocument from the file content:
        QDomDocument doc( QString::fromAscii( "charm_template" ) );
        QFile file( options.file() );
        if ( ! file.open( QIODevice::ReadOnly ) ) {
            throw Exception( QObject::tr( "Cannot open specified file for reading." ) );
        }
        QString errorMessage;
        int errorLine, errorColumn;
        if ( ! doc.setContent( &file, false, &errorMessage, &errorLine, &errorColumn ) ) {
            throw Exception( QObject::tr( "Cannot read specified file: " ) + errorMessage
                             + QObject::tr( " (%1:%2)").arg( errorLine ).arg( errorColumn ) );
        }

        // find the time sheet elements, loop to create the time sheets:
        QDomElement docElem = doc.documentElement();
        if ( docElem.tagName() != QString::fromAscii( "charm_template" ) ) {
            throw Exception( QObject::tr( "Wrong document element found in the specified file." ) );
        }
        const QString timesheetTagName = QString::fromAscii( "timesheet" );
        for ( QDomElement child = docElem.firstChildElement( timesheetTagName );
              ! child.isNull();
              child = child.nextSiblingElement( timesheetTagName ) ) {
            const QString userIdString = child.attributes().namedItem( "userid" ).nodeValue();
            bool ok;
            const int userId = userIdString.toInt( &ok );
            if ( !ok ) {
                throw Exception( QObject::tr( "Mis-spelled or missing user id in timesheet entry." ) );
            }

            // read event entries:
            EventList events;
            // FIXME readEfforts?
            const QString eventTagName = QString::fromAscii( "event" );
            for ( QDomElement eventElem = child.firstChildElement( eventTagName );
                  ! eventElem.isNull();
                  eventElem = eventElem.nextSiblingElement( eventTagName ) ) {
                try {
                    Event event = Event::fromXml( eventElem );
                    // event.dump();
                    events << event;
                } catch(  XmlSerializationException& e ) {
                    throw Exception( QObject::tr( "Error reading event: " ) + e.what() );
                }
            }
            // now create the time sheet:
            QDomDocument document = XmlSerialization::createXmlTemplate( "weekly-timesheet" );

            // find metadata and report element:
            QDomElement root = document.documentElement();
            QDomElement metadata = XmlSerialization::metadataElement( document );
            QDomElement report = XmlSerialization::reportElement( document );
            Q_ASSERT( !root.isNull() && !metadata.isNull() && !report.isNull() );
            // extend metadata tag: add year, and serial (week) number:
            // temp:
            // the start date of the week, specified on the command line:
            const QDateTime start( options.date(), QTime(), Qt::UTC );
            int year;
            const int week = start.date().weekNumber( &year );

            {
                QDomElement yearElement = document.createElement( "year" );
                metadata.appendChild( yearElement );
                QDomText text = document.createTextNode( QString::number( year ) );
                yearElement.appendChild( text );
                QDomElement weekElement = document.createElement( "serial-number" );
                weekElement.setAttribute( "semantics", "week-number" );
                metadata.appendChild( weekElement );
                QDomText weektext = document.createTextNode( QString::number( week ) );
                weekElement.appendChild( weektext );
            }

            {   // effort
                // make effort element:
                QDomElement effort = document.createElement( "effort" );
                report.appendChild( effort );
                // create elements:
                Q_FOREACH( Event event, events ) {
                    const QDateTime end = start.addSecs( event.duration() );
                    event.setStartDateTime( start );
                    event.setEndDateTime( end );
                    effort.appendChild( event.toXml( document ) );
                }
            }

            // save the file:
            // temp:
            const QString filename = QString::fromAscii( "WeeklyTimesheet-generated-%1-%2-%3.charmreport" )
                                     .arg( userId ).arg( year ).arg( week );
            QFile file( filename );
            if ( file.open( QIODevice::WriteOnly ) ) {
                QTextStream stream( &file );
                document.save( stream, 4 );
                cout << "Generated Time Sheet: " << qPrintable( filename ) << endl;
            } else {
                throw Exception( QObject::tr( "Error writing file " ) + filename );
            }
        }
        return 0;
    } catch( TimesheetGenerator::UsageException& e ) {
        cerr << e.what() << endl;
        cout << "Usage: " << endl
             << "   * TimesheetGenerator -h                              <-- get help"
             << endl
             << "   * TimesheetGenerator -f template-filename -d date    <-- generate timesheets from template for that date"
             << endl;
        return 1;
    } catch( TimesheetGenerator::Exception& e ) {
        cerr << e.what() << endl;
        return 1;
    }
}

