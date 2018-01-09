/*
  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2009-2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Frank Osterfeld <frank.osterfeld@kdab.com>

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

#ifndef TEMPORARYVALUE_H
#define TEMPORARYVALUE_H

template<typename T>
struct TemporaryValue {
    explicit TemporaryValue(T &x, const T &value)
        : m_x(x)
        , m_oldValue(x)
    {
        m_x = value;
    }

    ~TemporaryValue()
    {
        m_x = m_oldValue;
    }

    T &m_x;
    const T m_oldValue;
};

#endif
