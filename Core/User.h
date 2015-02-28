/*
  User.h

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

#ifndef USER_H
#define USER_H

#include <QString>

class User {
public:
    User()
        : m_id( 0 )
    {}

    User( const QString& name, int id )
        : m_name( name )
        , m_id( id )
    {}

    bool operator==( const User& other ) const
    {
        return m_name == other.m_name && m_id == other.m_id;
    }

    bool isValid() const {
        return m_id != 0;
    }

    const QString& name() const {
        return m_name;
    }

    void setName( const QString& newname ) {
        m_name = newname;
    }

    int id() const {
        return m_id;
    }

    void setId( int newid ) {
        m_id = newid;
    }

private:
    QString m_name;
    int m_id;
};

#endif
