#ifndef SQLSTORAGE_H
#define SQLSTORAGE_H

#include <QString>

#include "StorageInterface.h"

class QSqlDatabase;
class QSqlQuery;
class QSqlRecord;

class SqlStorage : public StorageInterface
{
public:
    SqlStorage();
    ~SqlStorage();

    void stateChanged( State previous );

    int makeEventId();

    virtual QSqlDatabase& database() = 0;

    // installation id handling
    Installation createInstallation( const QString& name );
    Installation getInstallation( int installationId );
    bool modifyInstallation( const Installation& );
    bool deleteInstallation( const Installation& );

    // implement user database functions:
    User getUser ( int userid );
    User makeUser( const QString& name );
    bool modifyUser ( const User& user );
    bool deleteUser ( const User& user );

    // implement task database functions:
    TaskList getAllTasks();
    bool setAllTasks( const User& user, const TaskList& tasks );
    bool addTask( const Task& task );
    bool addTask( const Task& task, const SqlRaiiTransactor& );
    Task getTask( int taskid );
    bool modifyTask( const Task& task );
    bool deleteTask( const Task& task );
    bool deleteAllTasks();
    bool deleteAllTasks( const SqlRaiiTransactor& );

    // implement event database functions:
    EventList getAllEvents();
    Event makeEvent();
    Event makeEvent( const SqlRaiiTransactor& );
    Event getEvent( int eventid );
    bool modifyEvent( const Event& event );
    bool modifyEvent( const Event& event, const SqlRaiiTransactor& );
    bool deleteEvent( const Event& event );
    bool deleteAllEvents();
    bool deleteAllEvents( const SqlRaiiTransactor& );

    // implement subscription management functions:
    bool addSubscription( User, Task );
    bool deleteSubscription( User, Task );

    // implement metadata management functions:
    bool setMetaData( const QString&,  const QString& );
    bool setMetaData( const QString&,  const QString&, const SqlRaiiTransactor& );

    QString getMetaData( const QString& );

    // implement import functions:
    QString setAllTasksAndEvents( const User&, const TaskList&, const EventList& );

    /**
     * @throws UnsupportedDatabaseVersionException
     */
    bool verifyDatabase();

    virtual bool createDatabaseTables() = 0;

    // run the query and process possible errors
    static bool runQuery( QSqlQuery& );

protected:
    virtual QString lastInsertRowFunction() const = 0;

private:
    Event makeEventFromRecord( const QSqlRecord& );
    Task makeTaskFromRecord( const QSqlRecord& );
};

#endif
