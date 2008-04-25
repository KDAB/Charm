/*
 * MySqlStorage.h
 *
 *  Created on: Apr 23, 2008
 *      Author: mirko
 */

#ifndef MYSQLSTORAGE_H_
#define MYSQLSTORAGE_H_

#include "SqlStorage.h"

class MySqlStorage: public SqlStorage
{
public:
	MySqlStorage();
	virtual ~MySqlStorage();

	QSqlDatabase& database();
	
	QString description() const;
	bool connect(Configuration&);
	bool disconnect();
	int installationId() const;
	bool createDatabase(Configuration&);
    bool createDatabaseTables();

protected:
	QString lastInsertRowFunction() const;

private:
    QSqlDatabase m_database;
};

#endif /* MYSQLSTORAGE_H_ */
