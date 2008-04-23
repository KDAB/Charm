#ifndef DATABASE_H_
#define DATABASE_H_

#include <QString>

class Database
{
public:
	Database();
	virtual ~Database();

	void login();
	
	bool checkUserid( int id );
};

#endif /*DATABASE_H_*/
