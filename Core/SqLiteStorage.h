#ifndef SQLITESTORAGE_H
#define SQLITESTORAGE_H

#include <QSqlDatabase>

#include "SqlStorage.h"

class Configuration;

class SqLiteStorage : public SqlStorage
{
public:
    SqLiteStorage();
    ~SqLiteStorage();

    QString description() const;
    bool connect( Configuration& );
    bool disconnect();

    QSqlDatabase& database();
    int installationId() const;

protected:
    bool createDatabase( Configuration& );
    bool createDatabaseTables();
    QString lastInsertRowFunction() const;

private:
    QSqlDatabase m_database;
    int m_installationId;
};

#endif
