/*
  CharmExceptions.cpp

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

#include <CharmExceptions.h>

CharmException::CharmException( const QString& message )
    : m_message( message )
{}

QString CharmException::what() const throw() {
    return m_message;
}

ParseError::ParseError( const QString& text )
    : CharmException( text )
{}

XmlSerializationException:: XmlSerializationException( const QString& message )
    : CharmException( message )
{}

UnsupportedDatabaseVersionException::UnsupportedDatabaseVersionException( const QString& message )
    : CharmException( message )
{}

InvalidTaskListException::InvalidTaskListException( const QString& message )
    : CharmException( message )
{}

TransactionException::TransactionException( const QString& text )
    : CharmException( text )
{}

AlreadyRunningException::AlreadyRunningException()
    : CharmException( QString() )
{}
