#ifndef SQLRAIITRANSACTOR_H
#define SQLRAIITRANSACTOR_H

class QSqlDatabase;

class SqlRaiiTransactor {
public:
    SqlRaiiTransactor( QSqlDatabase& database );
    ~SqlRaiiTransactor();

    bool isActive() const;

    bool commit();
private:
    bool m_active;
    QSqlDatabase& m_database;
};

#endif
