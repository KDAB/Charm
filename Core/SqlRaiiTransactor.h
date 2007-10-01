#ifndef SQLRAIITRANSACTOR_H
#define SQLRAIITRANSACTOR_H

#include <QSqlDatabase>

class SqlRaiiTransactor {
public:
    SqlRaiiTransactor( QSqlDatabase& database )
        : m_database ( database )
    {
        m_active = database.transaction();
    }

    ~SqlRaiiTransactor() {
        if ( m_active ) {
            if ( ! m_database.rollback() ) {
                // throw database_failure_exception
            }
        }
    }

    bool isActive() const {
        return m_active;
    }

    bool commit() {
        if ( m_active && m_database.commit() ) {
            m_active = false;
            return true;
        } else {
            return false;
        }
    }

private:
    bool m_active;
    QSqlDatabase& m_database;
};

#endif
