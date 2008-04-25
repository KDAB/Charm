#ifndef DATABASE_H_
#define DATABASE_H_

#include <QString>

#include "Core/MySqlStorage.h"
#include "Exceptions.h"

class Database
{
public:
	Database();
	virtual ~Database();

	void login() throw ( TimesheetProcessorException );
	void initializeDatabase() throw ( TimesheetProcessorException );
	bool checkUserid( int id );
private:
	MySqlStorage m_storage;
};

#endif /*DATABASE_H_*/
