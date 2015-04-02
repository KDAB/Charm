/*
  ImportExportTests.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2008-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>

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

#include "ImportExportTests.h"

#include "Core/Controller.h"
#include "Core/Task.h"
#include "Core/CharmDataModel.h"
#include "Charm/Commands/CommandImportFromXml.h"

#include <QtDebug>
#include <QString>
#include <QtTest/QtTest>
#include <QSharedPointer>
#include <QDomDocument>

ImportExportTests::ImportExportTests()
    : TestApplication("./ImportExportTestDatabase.db")
{
}

void ImportExportTests::initTestCase()
{
    initialize();
}

void ImportExportTests::importExportTest()
{
    const QString localFileName( "ImportExportTests-temp.charmdatabaseexport" );
    const QString filename = ":/importExportTest/Data/test-database-export.charmdatabaseexport";
    importDatabase( filename );

    QSharedPointer<CharmDataModel> databaseStep1( model()->clone() );

    QDomDocument exportDoc = controller()->exportDatabasetoXml();
    QFile outfile( localFileName );
    QVERIFY( outfile.open( QIODevice::ReadWrite ) );
    QTextStream stream( &outfile );
    exportDoc.save( stream, 4 ); // FIXME save does no kind of error reporting?

    importDatabase( filename );
    QCOMPARE( *databaseStep1.data(), *model() );


    // Code to load and anonymize a database:
//    QBENCHMARK {
//        Q_FOREACH( Task task, model()->getAllTasks() ) {
//            task.setName( QString::number( task.id() ) );
//            QVERIFY( controller()->modifyTask( task ) );
//        }

//        EventMap eventMap = model()->eventMap();
//        for( EventMap::const_iterator it = eventMap.begin(); it != eventMap.end(); ++it ) {
//            Event event = (*it).second;
//            event.setComment( QString::number( event.id() ));
//            QVERIFY( controller()->modifyEvent( event ) );
//        }

//        QDomDocument exportDoc = controller()->exportDatabasetoXml();
//        QFile outfile( "test-database-export.charmdatabaseexport" );
//        QVERIFY( outfile.open( QIODevice::ReadWrite ) );
//        QTextStream stream( &outfile );
//        exportDoc.save( stream, 4 );
//        qDebug() << outfile.fileName();
//    }
}

void ImportExportTests::importBenchmark()
{
    const QString filename = ":/importExportTest/Data/test-database-export.charmdatabaseexport";
    QBENCHMARK {
        importDatabase( filename );
    }
}

void ImportExportTests::exportBenchmark()
{
    const QString filename = ":/importExportTest/Data/test-database-export.charmdatabaseexport";
    const QString localFileName( "ImportExportTests-temp.charmdatabaseexport" );
    importDatabase( filename );
    QBENCHMARK {
        QDomDocument exportDoc = controller()->exportDatabasetoXml();
        QFile outfile( localFileName );
        QVERIFY( outfile.open( QIODevice::ReadWrite ) );
        QTextStream stream( &outfile );
        exportDoc.save( stream, 4 ); // FIXME save does no kind of error reporting?
    }
}

void ImportExportTests::cleanupTestCase ()
{
    destroy();
}

void ImportExportTests::importDatabase( const QString& filename )
{
    QFile file( filename );
    QVERIFY( file.open( QIODevice::ReadOnly ) );
    QDomDocument dom;
    QVERIFY( dom.setContent( &file ) );
    QVERIFY( controller()->importDatabaseFromXml( dom ).isEmpty() );
}


QTEST_MAIN( ImportExportTests )

#include "moc_ImportExportTests.cpp"
