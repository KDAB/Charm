#include <QIcon>
#include <QPixmap>

#include "Data.h"

const QIcon& Data::charmIcon()
{
    Q_ASSERT_X( ! QPixmap( ":/Charm/charmicon.png" ).isNull(), "Data::charmIcon",
                "Required resource not available" );
    static QIcon icon( QPixmap( ":/Charm/charmicon.png" ) );
    return icon;
}

const QIcon& Data::goIcon()
{
    Q_ASSERT_X( ! QPixmap( ":/Charm/go.png" ).isNull(), "Data::goIcon",
                "Required resource not available" );
    static QIcon icon( QPixmap( ":/Charm/go.png" ) );
    return icon;
}

const QIcon& Data::stopIcon()
{
    Q_ASSERT_X( ! QPixmap( ":/Charm/stop.png" ).isNull(), "Data::stopIcon",
                "Required resource not available" );
    static QIcon icon( QPixmap( ":/Charm/stop.png" ) );
    return icon;
}

const QIcon& Data::newTaskIcon()
{
    Q_ASSERT_X( !QPixmap( ":/Charm/new.png" ).isNull(), "Data::newTaskIcon",
                "Required resource not available" );
    static QIcon icon( QPixmap( ":/Charm/new.png" ) );
    return icon;
}

const QIcon& Data::deleteTaskIcon()
{
    Q_ASSERT_X( !QPixmap( ":/Charm/deletetask.png" ).isNull(), "Data::deleteTaskIcon",
                "Required resource not available" );
    static QIcon icon( QPixmap( ":/Charm/deletetask.png" ) );
    return icon;
}

const QIcon& Data::previousEventIcon()
{
    Q_ASSERT_X( !QPixmap( ":/Charm/previous.png" ).isNull(), "Data::previousEventIcon",
                "Required resource not available" );
    static QIcon icon( QPixmap( ":/Charm/previous.png" ) );
    return icon;
}

const QIcon& Data::nextEventIcon()
{
    Q_ASSERT_X( !QPixmap( ":/Charm/next.png" ).isNull(), "Data::previousEventIcon",
                "Required resource not available" );
    static QIcon icon( QPixmap( ":/Charm/next.png" ) );
    return icon;
}

const QPixmap& Data::activePixmap()
{
    static QPixmap pixmap( ":/Charm/active.png" );
    Q_ASSERT_X( !pixmap.isNull(), "Data::activePixmap",
                "Required resource not available" );
    return pixmap;
}

const QPixmap& Data::checkIcon()
{
    static QPixmap pixmap( ":/Charm/check.png" );
    Q_ASSERT_X( !pixmap.isNull(), "Data::checkIcon",
                "Required resource not available" );
    return pixmap;
}

const QIcon& Data::quitCharmIcon()
{
    Q_ASSERT_X( ! QPixmap( ":/Charm/quitcharm.png" ).isNull(), "Data::quitCharmIcon",
                "Required resource not available" );
    static QIcon icon( QPixmap( ":/Charm/quitcharm.png" ) );
    return icon;
}

const QIcon& Data::clearFilterIcon()
{
    Q_ASSERT_X( ! QPixmap( ":/Charm/clearfilter.png" ).isNull(), "Data::quitCharmIcon",
                "Required resource not available" );
    static QIcon icon( QPixmap( ":/Charm/clearfilter.png" ) );
    return icon;
}

const QIcon& Data::configureIcon()
{
    Q_ASSERT_X( ! QPixmap( ":/Charm/configure.png" ).isNull(), "Data::quitCharmIcon",
                "Required resource not available" );
    static QIcon icon( QPixmap( ":/Charm/configure.png" ) );
    return icon;
}

const QPixmap& Data::editorLockedPixmap()
{
    static QPixmap pixmap( ":/Charm/editor_locked.png" );
    Q_ASSERT_X( !pixmap.isNull(), "Data::editorLockedPixmap",
                "Required resource not available" );
    return pixmap;
}

const QPixmap& Data::editorDirtyPixmap()
{
    static QPixmap pixmap( ":/Charm/editor_dirty.png" );
    Q_ASSERT_X( !pixmap.isNull(), "Data::editorDirtyPixmap",
                "Required resource not available" );
    return pixmap;
}

const QPixmap& Data::backdropPixmap()
{
    static QPixmap pixmap( ":/Backdrops/backdrop1.jpg" );
    Q_ASSERT_X( !pixmap.isNull(), "Data::editorDirtyPixmap",
                "Required resource not available" );
    return pixmap;
}


