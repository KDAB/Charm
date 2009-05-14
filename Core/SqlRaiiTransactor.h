#ifndef SQLRAIITRANSACTOR_H
#define SQLRAIITRANSACTOR_H

#include <QSqlDatabase>
#include <QSqlError>

class SqlRaiiTransactor {
public:
    SqlRaiiTransactor( QSqlDatabase& database )
        : m_database ( database )
    {
        m_active = m_database.transaction();
        if ( ! m_active ) {
            qWarning() << "Failed to begin transaction: " << m_database.lastError().text();
        }
    }

    ~SqlRaiiTransactor() {
        if ( m_active ) {
            if ( ! m_database.rollback() ) {
                qWarning() << "Failed to rollback transaction: " << m_database.lastError().text();
                // throw database_failure_exception
            }
        }
    }

    bool isActive() const {
        return m_active;
    }

    bool commit() {
        if ( m_active ) {
            if ( m_database.commit() ) {
                m_active = false;
                return true;
            }
            qWarning() << "Failed to commit transaction: " << m_database.lastError().text();
        }
        return false;
    }

private:
    bool m_active;
    QSqlDatabase& m_database;
};

#endif
