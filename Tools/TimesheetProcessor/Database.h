#ifndef DATABASE_H_
#define DATABASE_H_

#include <QString>

#include "Core/User.h"
#include "Core/Task.h"
#include "Core/MySqlStorage.h"
#include "Exceptions.h"

class Database
{
public:
    Database();
    virtual ~Database();

    void login() throw ( TimesheetProcessorException );
    void initializeDatabase() throw ( TimesheetProcessorException );
    void addEvent( const Event& event );
    void deleteEventsForReport ( int userid, int index );
    void checkUserid( int id ) throw (TimesheetProcessorException );
    User getOrCreateUserByName( QString name ) throw (TimesheetProcessorException );
    Task getTask( int taskid ) throw (TimesheetProcessorException );
    TaskList getAllTasks() throw (TimesheetProcessorException );

    QSqlDatabase& database();

private:
    MySqlStorage m_storage;
};

#endif /*DATABASE_H_*/
