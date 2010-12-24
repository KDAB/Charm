#include <QtDebug>
#include <QString>
#include <QtTest/QtTest>
#include <QSharedPointer>
#include <QDomDocument>

#include "Core/Controller.h"
#include "Core/Task.h"
#include "Core/CharmDataModel.h"
#include "Charm/Commands/CommandImportFromXml.h"

#include "ImportExportTests.h"

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

#include "ImportExportTests.moc"

