#include <QtDebug>
#include <QString>
#include <QtTest/QtTest>
#include <QDomDocument>

#include "Core/Controller.h"
#include "Core/Task.h"
#include "Core/CharmDataModel.h"
#include "Charm/Commands/CommandImportFromXml.h"

#include "ImportExportTests.h"

ImportExportTests::ImportExportTests()
    : TestApplication()
{
}

void ImportExportTests::initTestCase()
{
    initialize();
}

void ImportExportTests::importExportTest()
{
    const QString filename = ":/importExportTest/Data/test-database-export.charmdatabaseexport";
    QFile file( filename );
    QVERIFY( file.open( QIODevice::ReadOnly ) );
    QDomDocument dom;
    QVERIFY( dom.setContent( &file ) );
    QBENCHMARK {
        QVERIFY( controller()->importDatabaseFromXml( dom ).isEmpty() );

    }

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

void ImportExportTests::cleanupTestCase ()
{
    destroy();
}

QTEST_MAIN( ImportExportTests )

#include "ImportExportTests.moc"
