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

#include "SmartNameCacheTests.moc"
