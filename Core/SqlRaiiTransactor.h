#ifndef SQLRAIITRANSACTOR_H
#define SQLRAIITRANSACTOR_H

#include <exception>

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>

class SqlRaiiTransactor {
public:
    class TransactionException : public std::exception {
    public:
        explicit TransactionException( const QString& text = QString() )
            : mWhat( text )
        {}

        ~TransactionException() throw()
        {}

        const char* what() const throw() {
            return qPrintable( mWhat );
        }
    private:
        QString mWhat;
    };

    SqlRaiiTransactor( QSqlDatabase& database )
        : m_database ( database )
    {
        if ( ! database.driver()->hasFeature( QSqlDriver::Transactions ) ) {
            throw TransactionException( QObject::tr( "Database driver does not support transactions." ) );
        }
        m_active = m_database.transaction();
        if ( ! m_active ) {
            qWarning() << "Failed to begin transaction: " << m_database.lastError().text();
            throw TransactionException( QObject::tr( "Starting a transaction failed." ) );
        }
    }

    ~SqlRaiiTransactor() {
        if ( m_active ) {
            if ( ! m_database.rollback() ) {
                throw TransactionException( QObject::tr( "Database error, transaction rollback failed." ) );
                qWarning() << "Failed to rollback transaction: " << m_database.lastError().text();
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
            throw TransactionException( QObject::tr( "Failed to commit transaction: " ) + m_database.lastError().text() );
        }
        return false;
    }

private:
    bool m_active;
    QSqlDatabase& m_database;
};

#endif
