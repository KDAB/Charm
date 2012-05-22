#include <QtDebug>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>

#include "CharmExceptions.h"
#include "SqlRaiiTransactor.h"

SqlRaiiTransactor::SqlRaiiTransactor( QSqlDatabase& database )
    : m_database ( database )
{
    if ( ! database.driver()->hasFeature( QSqlDriver::Transactions ) ) {
        throw TransactionException( QObject::tr( "Database driver does not support transactions." ) );
    }
    m_active = m_database.transaction();
    if ( ! m_active ) {
        throw TransactionException( QObject::tr( "Starting a transaction failed: %1" ).arg( m_database.lastError().text() ) );
    }
}

SqlRaiiTransactor::~SqlRaiiTransactor() {
    if ( m_active ) {
        if ( ! m_database.rollback() ) {
            qWarning() << "Failed to rollback transaction: " << m_database.lastError().text();
        }
    }
}

bool SqlRaiiTransactor::isActive() const {
    return m_active;
}

bool SqlRaiiTransactor::commit() {
    if ( m_active ) {
        if ( m_database.commit() ) {
            m_active = false;
            return true;
        }
        throw TransactionException( QObject::tr( "Failed to commit transaction: " ) + m_database.lastError().text() );
    }
    return false;
}
