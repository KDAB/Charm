#include <QDateTime>
#include <QtDebug>
#include <QtTest/QtTest>

#include "XmlSerializationTests.h"
#include "Event.h"

XmlSerializationTests::XmlSerializationTests()
    : QObject()
{
}

void XmlSerializationTests::testEventSerialization()
{
    // set up test candidates:
    EventList eventsToTest;
    Event event;
    event.setComment( "A comment" );
    event.setStartDateTime( QDateTime::currentDateTime() );
    event.setEndDateTime( QDateTime::currentDateTime().addDays( 1 ) );
    // add one default-constructed one, plus the other candidates:
    eventsToTest << Event() << event;

    QDomDocument document( "testdocument" );
    Q_FOREACH( Event event, eventsToTest ) {
        QDomElement element = event.toXml( document );
//         // temp:
//         document.appendChild( element );
//         qDebug() << document.toString( 4 );
//         // ^^^
        try {
            Event readEvent = Event::fromXml( element );
            // the extra tests are mostly to immidiately see what is wrong:
            QVERIFY( event.comment() == readEvent.comment() );
            QVERIFY( event.startDateTime() == readEvent.startDateTime() );
            QVERIFY( event.endDateTime() == readEvent.endDateTime() );
            QVERIFY( event == readEvent );
        } catch( std::exception& e ) {
            qDebug() << "XmlSerializationTests::testEventSerialization: exception caught ("
                     << e.what() << ")";
            QFAIL( "Event Serialization throws" );
        }
    }
}

void XmlSerializationTests::testTaskSerialization()
{
    // set up test candidates:
    TaskList tasksToTest;
    Task task;
    task.setName( "A task" );
    task.setId( 42 );
    task.setParent( 4711 );
    task.setSubscribed( true );
    Task task2;
    task2.setName( "Another task" );
    task2.setId( -1 );
    task2.setParent( 1000000000 );
    task2.setSubscribed( false );
    task2.setValidFrom( QDateTime::currentDateTime() );
    task2.setValidUntil( QDateTime::currentDateTime() );
    Task task3;

    tasksToTest << Task() << task << task2;

    QDomDocument document( "testdocument" );
    Q_FOREACH( Task task, tasksToTest ) {
        QDomElement element = task.toXml( document );
        try {
            Task readTask = Task::fromXml( element );
            QVERIFY( task == readTask );
        } catch( std::exception& e ) {
            qDebug() << "XmlSerializationTests::testTaskSerialization: exception caught ("
                     << e.what() << ")";
            QFAIL( "Task Serialization throws" );
        }
    }
}

void XmlSerializationTests::testQDateTimeToFromString()
{
    // test regular QDate::toString:
    QTime time1( QTime::currentTime() );
    time1.setHMS( time1.hour(), time1.minute(), time1.second() ); // strip milliseconds
    QString time1string( time1.toString() );
    QTime time2 = QTime::fromString( time1string );
    QVERIFY( time1 == time2 );

    // test toString with ISODate:
    QTime time3( QTime::currentTime() );
    time3.setHMS( time3.hour(), time3.minute(), time3.second() ); // strip milliseconds
    QString time3string( time3.toString( Qt::ISODate) );
    QTime time4 = QTime::fromString( time3string, Qt::ISODate );
    QVERIFY( time3 == time4 );

    // test regular QDateTime::toString:
    QDateTime date1( QDateTime::currentDateTime() );
    date1.setTime( time1 );
    QString date1string = date1.toString();
    QDateTime date2 = QDateTime::fromString( date1string );
    QVERIFY( date1 == date2 );

    // test regular QDateTime::toString:
    QDateTime date3( QDateTime::currentDateTime() );
    date3.setTime( time1 );
    QString date3string = date3.toString( Qt::ISODate );
    QDateTime date4 = QDateTime::fromString( date3string, Qt::ISODate );
    QVERIFY( date3 == date4 );
}

QTEST_MAIN( XmlSerializationTests )

#include "XmlSerializationTests.moc"
