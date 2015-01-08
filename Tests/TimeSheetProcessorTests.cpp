#include "TimeSheetProcessorTests.h"
#include <QtTest/QtTest>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

#include "Tools/TimesheetProcessor/Operations.h"
#include "Tools/TimesheetProcessor/CommandLine.h"
#include "Core/MySqlStorage.h"
#include <QDebug>

#include <QDateTime>

TimeSheetProcessorTests::TimeSheetProcessorTests():
    m_idTimeSheet(0),
    m_adminId(43),
    m_reportPath(":/timeSheetProcessorTest/Data/test-timesheet-report.charmreport")
{
}

void TimeSheetProcessorTests::testAddRemoveTimeSheet()
{
    // Add and Remove in the same function to avoid order dependency
    // TimesheetProcessor -a filename -u userid -m comment  <-- add timesheet from file

    // GIVEN
    Q_ASSERT( !m_reportPath.isEmpty() );
    CommandLine cmdAdd( m_reportPath, m_adminId );

    // WHEN
    addTimesheet( cmdAdd );

    // THEN
    MySqlStorage storage;
    MySqlStorage::Parameters parameters = MySqlStorage::parseParameterEnvironmentVariable();
    storage.configure( parameters );
    QVERIFY( storage.database().open() );

    QSqlQuery query( storage.database() );
    query.prepare("SELECT id, date_time_uploaded from timesheets where filename=:file AND userid=:user");
    query.bindValue( "file", m_reportPath );
    query.bindValue( "user", m_adminId );
    QVERIFY( storage.runQuery( query ) );
    QVERIFY( query.next() );

    QSqlRecord record = query.record();
    m_idTimeSheet = record.value( record.indexOf("id") ).toInt();
    uint dateTimeUploaded = record.value( record.indexOf("date_time_uploaded") ).toInt();
    uint nowTimeStamp = (QDateTime::currentMSecsSinceEpoch()/1000);// seconds since 1970-01-01

    QVERIFY( m_idTimeSheet > 0 );
    QVERIFY( dateTimeUploaded > (nowTimeStamp - 60*60) ) ; // one hour ago
    QVERIFY( dateTimeUploaded < (nowTimeStamp + 60*60) ); // in one hour

    // GIVEN
    Q_ASSERT(m_idTimeSheet != 0);
    CommandLine cmdRemove(m_adminId, m_idTimeSheet);

    // WHEN
    removeTimesheet(cmdRemove);

    // THEN
    MySqlStorage storageRemove;
    storageRemove.configure( parameters );
    QVERIFY( storageRemove.database().open() );

    QSqlQuery queryRemove( storageRemove.database() );
    queryRemove.prepare("SELECT id, date_time_uploaded from timesheets where filename=:file AND userid=:user");
    queryRemove.bindValue( "file", m_reportPath );
    queryRemove.bindValue( "user", m_adminId );
    QVERIFY( storageRemove.runQuery( queryRemove ) );
    QVERIFY( !queryRemove.next() ); // not retrievable since it was deleted, must return false
}

QTEST_MAIN( TimeSheetProcessorTests)

#include "moc_TimeSheetProcessorTests.cpp"
