/*
  CharmQtCompat.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2017-2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Hannah von Reth <hannah.vonreth@kdab.com>

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
#ifndef CHARMQTCOMPAT_H
#define CHARMQTCOMPAT_H

#include <QtGlobal>

#if QT_VERSION < QT_VERSION_CHECK(5, 7, 0)

QT_BEGIN_NAMESPACE

// this adds const to non-const objects (like std::as_const)
template <typename T>
Q_DECL_CONSTEXPR typename std::add_const<T>::type &qAsConst(T &t) Q_DECL_NOTHROW { return t; }

// prevent rvalue arguments:
template <typename T>
void qAsConst(const T &&) Q_DECL_EQ_DELETE;

QT_END_NAMESPACE

#endif

#endif // CHARMQTCOMPAT_H
