/*
  SqlRaiiTransactor.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "SqlRaiiTransactor.h"
#include "CharmExceptions.h"

#include <QtDebug>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>

SqlRaiiTransactor::SqlRaiiTransactor( QSqlDatabase& database )
    : m_active( false ),
      m_database ( database )
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
