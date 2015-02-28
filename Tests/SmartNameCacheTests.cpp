/*
  SmartNameCacheTests.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2012-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

#include "SmartNameCacheTests.h"
#include "Core/SmartNameCache.h"

#include <QtTest/QtTest>


void SmartNameCacheTests::testCache()
{
    SmartNameCache cache;
    Task projects( 1, QLatin1String("Projects") );
    Task charm( 2, QLatin1String("Charm") );
    charm.setParent( projects.id() );
    Task charmDevelopment( 3, QLatin1String("Development") );
    charmDevelopment.setParent( charm.id() );
    Task charmOverhead( 4, QLatin1String("Overhead") );
    charmOverhead.setParent( charm.id() );
    Task lotsofcake( 5, QLatin1String("Lotsofcake") );
    lotsofcake.setParent( projects.id() );
    Task lotsofcakeDevelopment( 6, QLatin1String("Development") );
    lotsofcakeDevelopment.setParent( lotsofcake.id() );
    const TaskList tasks = TaskList() << projects << charm << charmDevelopment << charmOverhead << lotsofcake << lotsofcakeDevelopment;
    cache.setAllTasks( tasks );
    QCOMPARE( cache.smartName( charmDevelopment.id() ), QLatin1String("Charm/Development") );
    QCOMPARE( cache.smartName( charmOverhead.id() ), QLatin1String("Charm/Overhead") );
    QCOMPARE( cache.smartName( projects.id() ), QLatin1String("Projects") );
    QCOMPARE( cache.smartName( lotsofcakeDevelopment.id() ), QLatin1String("Lotsofcake/Development") );
}


QTEST_MAIN( SmartNameCacheTests )

#include "moc_SmartNameCacheTests.cpp"
