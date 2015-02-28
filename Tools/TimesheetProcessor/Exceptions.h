/*
  Exceptions.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2008-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <QString>

class TimesheetProcessorException : public std::exception
{
public:
    explicit TimesheetProcessorException(const QString& text = QString::null) :
        mWhat(text)
    {
    }

    ~TimesheetProcessorException() throw()
    {
    }

    const char* what() const throw()
    {
        return qPrintable(mWhat);
    }

private:
    QString mWhat;
};

class UsageException : public TimesheetProcessorException
{
public:
    explicit UsageException(const QString& text = QString::null) :
        TimesheetProcessorException(text)
    {
    }
};

#endif
