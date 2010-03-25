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
    struct Parameters {
        Parameters() : port( 3309), database( "Charm" ) {}
        unsigned int port;
        QString database;
        QString name;
        QString password;
        QString host;
    };

    MySqlStorage();
    virtual ~MySqlStorage();

    QSqlDatabase& database();

    QString description() const;
    bool connect(Configuration&);
    bool disconnect();
    int installationId() const;
    bool createDatabase(Configuration&);
    bool createDatabaseTables();

    static Parameters parseParameterEnvironmentVariable();
    void configure( const Parameters& );
protected:
    QString lastInsertRowFunction() const;

private:
    QSqlDatabase m_database;
};

#endif /* MYSQLSTORAGE_H_ */
