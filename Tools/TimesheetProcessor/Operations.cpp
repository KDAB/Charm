/*
  Operations.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2008-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>
  Author: Sebastian Sauer <sebastian.sauer@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Operations.h"
#include "CommandLine.h"
#include "Exceptions.h"
#include "Database.h"

#include "Core/User.h"
#include "Core/Event.h"
#include "Core/SqlRaiiTransactor.h"
#include "Core/XmlSerialization.h"

#include <QtDebug>
#include <QFile>
#include <QObject>
#include <QDomDocument>
#include <QSqlDatabase>
#include <QVariant>
#include <QSqlQuery>
#include <QSqlRecord>

#include <iostream>

void initializeDatabase(const CommandLine& cmd)
{
        using namespace std;

        cout << "Initializing database." << endl;

        Database database;
        database.login();
        cout << "Logged in." << endl;

        database.initializeDatabase();

        cout << "Database initialized." << endl;
}

void checkOrCreateUser(const CommandLine& cmd)
{
        using namespace std;
        Database database;
        database.login();
        User user = database.getOrCreateUserByName( cmd.userName( ));
        cout << user.id() << endl;
}

void addTimesheet(const CommandLine& cmd)
{
    using namespace std;

    // load the time sheet:
    QFile file(cmd.filename() );
    if ( !file.exists() )
    {
        throw TimesheetProcessorException( QObject::tr("File %1 does not exist.").arg(cmd.filename() ));
    }
    // load the XML into a DOM tree:
    if (!file.open(QIODevice::ReadOnly))
    {
        QString msg = QObject::tr("Cannot open file %1 for reading.").arg(cmd.filename());
        throw TimesheetProcessorException( msg);
    }
    QDomDocument doc("timesheet");
    if (!doc.setContent(&file))
    {
        QString msg = QObject::tr("Cannot read file %1.").arg(cmd.filename());
        throw TimesheetProcessorException( msg);
    }
    // add it to the database:
    // 1) make a list of all the events:
    EventList events;
    QDomElement charmReportElement = doc.firstChildElement("charmreport");
    QDomElement metadataElement = charmReportElement.firstChildElement("metadata");
    QDomElement yearElement = metadataElement.firstChildElement("year");
    QString year = yearElement.text().simplified();
    QDomElement weekElement = metadataElement.firstChildElement("serial-number");
    QString week = weekElement.text().simplified();
    QDomElement reportElement = charmReportElement.firstChildElement("report");
    QDomElement effortElement = reportElement.firstChildElement("effort");
    if( effortElement.isNull() )
    {
        QString msg = QObject::tr("Invalid structure in file %1.").arg(cmd.filename());
        throw TimesheetProcessorException( msg);
    }

    int totalSeconds = 0;
    QDomElement element = effortElement.firstChildElement( Event::tagName() );
    for (; !element.isNull(); element = element.nextSiblingElement( Event::tagName() ) )
    {
        try {
            Event e = Event::fromXml(element);
            events << e;
            totalSeconds += e.duration();
            // e.dump();
        } catch ( const XmlSerializationException& e ) {
            const QString msg = QObject::tr("Syntax error in file %1: %2.").arg( cmd.filename(), e.what() );
            throw TimesheetProcessorException( msg );
        }
    }

    uint dateTimeUploaded = QDateTime::currentMSecsSinceEpoch()/1000;// seconds since 1970-01-01

    // 2) log into database
    Database database;
    database.login();
    int index = -1;

    // check for the user id
    database.checkUserid(cmd.userid());

    try {
        SqlRaiiTransactor transaction( database.database() );

        // add time sheet to time sheets list
        {
            QSqlQuery query( database.database() );
            query.prepare( "INSERT into timesheets VALUES( 0, :filename, :original_filename, :year, :week, :total, :userid, 0, :date_time_uploaded)" );
            query.bindValue( QString::fromLatin1( ":filename" ), cmd.filename() );
            query.bindValue( QString::fromLatin1( ":original_filename" ), cmd.userComment() );
            query.bindValue( QString::fromLatin1( ":year" ), year );
            query.bindValue( QString::fromLatin1( ":week" ), week );
            query.bindValue( QString::fromLatin1( ":total" ), totalSeconds );
            query.bindValue( ":userid", cmd.userid() );
            query.bindValue( QString::fromLatin1( ":date_time_uploaded" ), dateTimeUploaded );
            if ( ! query.exec() ) {
                QString msg = QObject::tr( "Error adding time sheet %1.").arg(cmd.filename() );
                throw TimesheetProcessorException( msg );
            }
        }

        // retrieve index
        {
            QSqlQuery query( database.database() );
            if ( ! query.exec( "SELECT id from timesheets WHERE id = last_insert_id()" ) ) {
                QString msg = QObject::tr( "SQL error retrieving index for time sheet %1.").arg(cmd.filename() );
                throw TimesheetProcessorException( msg );
            }

            if ( query.next() ) {
                const int idField = query.record().indexOf( "id" );
                index = query.value( idField ).toInt();
            } else {
                QString msg = QObject::tr( "Error retrieving index for time sheet %1.").arg(cmd.filename() );
                throw TimesheetProcessorException( msg );
            }
        }

        Q_ASSERT( index > 0 );

        cout << "Adding report " << index << " for user " << cmd.userid() << endl;

        // add the events to the database
        Q_FOREACH( Event e, events )
        {
            // check for the project code, if this does not throw an exception, the task id exists
            Task task = database.getTask( e.taskId());
            // FIXME check for reporting period for the task, not implemented in the DB
            e.setUserId( cmd.userid() );
            e.setReportId( index );
            database.addEvent( e, transaction );
        }

        transaction.commit();

        cout << "Report added" << endl
                << "total:" << totalSeconds << endl
                << "year:" << year.toLocal8Bit().constData() << endl
                << "week:" << week.toLocal8Bit().constData() << endl
                << "uploadedTime:" << dateTimeUploaded << endl
                << "index:" << index << endl;

    } catch ( const TimesheetProcessorException& e ) {
        if ( index >= 0 ) {
            // A valid index means that something with the events/tasks went wrong and triggered the
            // exception. In that case we already created an entry in the Charm/timesheets table that
            // we would need to remove again cause adding the timesheet just failed. Normally this
            // should have been done by the transaction we created above. Cause we did not commit()
            // the transction should be rollback() and there should be no item with id=index in the
            // Charm/timesheets table any longer...
            QSqlQuery query( database.database() );
            if ( query.exec( QString("SELECT id from timesheets WHERE id = %1").arg(index) ) ) {
                if ( query.next() ) { //WTF, if that happens that something went wrong with the transaction
                    QSqlQuery deletequery( database.database() );
                    deletequery.exec( QString("DELETE FROM timesheets WHERE id = %1").arg(index) );
                    qDebug() << "CRITICAL ERROR: A database transaction did not roll back as expected. "
                            "Please report this to the administrators. The time sheet with index " << index << " needs to be cleand up.";
                }
            }
        }
        throw e;
    }
}

void removeTimesheet(const CommandLine& cmd)
{
        using namespace std;
        cout << "Removing report " << cmd.index() << endl;

        Database database;
        database.login();
        SqlRaiiTransactor transaction( database.database() );
        database.deleteEventsForReport( cmd.userid(), cmd.index() );

        {
                QSqlQuery query( database.database() );
                if ( ! query.prepare( "DELETE from timesheets WHERE id = :index" ) ) {
                        QString msg = QObject::tr( "Error prepare to remove timesheet %1.").arg(cmd.index() );
                        throw TimesheetProcessorException( msg );
                }

                query.bindValue( QString::fromLatin1( ":index" ), cmd.index() );

                if ( ! query.exec() ) {
                        QString msg = QObject::tr( "Error removing timesheet %1.").arg(cmd.index() );
                        throw TimesheetProcessorException( msg );
                }

                if ( query.numRowsAffected() < 1 ) {
                        QString msg = QObject::tr( "No such timesheet %1.").arg(cmd.index() );
                        throw TimesheetProcessorException( msg );
                }
        }

        if ( ! transaction.commit() ) {
                QString msg = QObject::tr( "Error commit remove timesheet %1.").arg(cmd.index() );
                throw TimesheetProcessorException( msg );
        }

        cout << "Report " << cmd.index() << " removed" << endl;
}

void exportProjectcodes( const CommandLine& cmd )
{
    using namespace std;

    cout << "Exporting project codes to " << qPrintable( cmd.exportFilename() ) << endl;

    Database database;
    database.login();

    TaskList tasks = database.getAllTasks();
    try {
        TaskExport::writeTo( cmd.exportFilename(), tasks );
    } catch ( const XmlSerializationException& e ) {
        throw TimesheetProcessorException( QObject::tr( "Cannot write to file %1: %2" ).arg( cmd.exportFilename(), e.what() ) );
    }

    cout << "Done, " << tasks.count() << " tasks definitions exported." << endl;
}
