/*
  CharmExceptions.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2007-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

#ifndef CHARMEXCEPTIONS_H
#define CHARMEXCEPTIONS_H

#include <QString>

class CharmException
{
public:
    explicit CharmException( const QString& message );
    QString what() const throw();
private:
    QString m_message;
};

class ParseError : public CharmException {
public:
    explicit ParseError( const QString& text );
};

class XmlSerializationException : public CharmException
{
public:
    explicit XmlSerializationException( const QString& message );
};

class UnsupportedDatabaseVersionException : public CharmException
{
public:
    explicit UnsupportedDatabaseVersionException( const QString& message );
};

class InvalidTaskListException : public CharmException
{
public:
    explicit InvalidTaskListException( const QString& message );
};

class TransactionException : public CharmException {
public:
    explicit TransactionException( const QString& text = QString() );
};

class AlreadyRunningException : public CharmException {
public:
    explicit AlreadyRunningException();
};

#endif
