#include <QFile>
#include <QIcon>
#include <QPixmap>

#include "Data.h"

const QIcon& Data::charmIcon()
{
    Q_ASSERT_X(!QPixmap(":/Charm/charmicon.png").isNull(), "Data::charmIcon",
            "Required resource not available");
    static QIcon icon(QPixmap(":/Charm/charmicon.png"));
    return icon;
}

const QIcon& Data::charmTrayIcon()
{
    // Mac and Windows: the system tray uses 16x16.
    // TODO: different icons though: white background on Windows
    // On X11: pure-Qt apps get 22x22 from QSystemTrayIcon.
    // KDE apps seem to get 24x24 in KSystemTrayIcon via KIconLoader, which is actually better.

#ifdef Q_WS_MAC
    static const QString iconPath = QLatin1String(":/Charm/charmtray_mac.png");
#else
    static const QString iconPath = QLatin1String(":/Charm/charmtray22.png");
#endif

    Q_ASSERT_X(!QPixmap(iconPath).isNull(), "Data::charmTrayIcon",
                        "Required resource not available");
    static QIcon icon = QPixmap(iconPath);
    return icon;
}

const QIcon& Data::charmTrayActiveIcon()
{
    // Mac and Windows: the system tray uses 16x16.
    // TODO: different icons though: white background on Windows
    // On X11: pure-Qt apps get 22x22 from QSystemTrayIcon.
    // KDE apps seem to get 24x24 in KSystemTrayIcon via KIconLoader, which is actually better.

#ifdef Q_WS_MAC
    static const QString iconPath = QLatin1String(":/Charm/charmtrayactive_mac.png");
#else
    static const QString iconPath = QLatin1String(":/Charm/charmtray22.png");
#endif

    Q_ASSERT_X(!QPixmap(iconPath).isNull(), "Data::charmTrayActiveIcon",
                        "Required resource not available");
    static QIcon icon = QPixmap(iconPath);
    return icon;
}
const QIcon& Data::goIcon()
{
    Q_ASSERT_X(!QPixmap(":/Charm/go.png").isNull(), "Data::goIcon",
              "Required resource not available");
    static QIcon icon(QPixmap(":/Charm/go.png"));
    return icon;
}

const QIcon& Data::stopIcon()
{
    Q_ASSERT_X(!QPixmap(":/Charm/stop.png").isNull(), "Data::stopIcon",
            "Required resource not available");
    static QIcon icon(QPixmap(":/Charm/stop.png"));
    return icon;
}

const QIcon& Data::newTaskIcon()
{
    Q_ASSERT_X(!QPixmap(":/Charm/newtask.png").isNull(), "Data::newTaskIcon",
               "Required resource not available");
    static QIcon icon(QPixmap(":/Charm/newtask.png"));
    return icon;
}

const QIcon& Data::newSubtaskIcon()
{
    Q_ASSERT_X(!QPixmap(":/Charm/newsubtask.png").isNull(), "Data::newSubtaskIcon",
               "Required resource not available");
    static QIcon icon(QPixmap(":/Charm/newsubtask.png"));
    return icon;
}

const QIcon& Data::editTaskIcon()
{   // FIXME same as edit-event icon
    Q_ASSERT_X(!QPixmap(":/Charm/edit.png").isNull(), "Data::editTaskIcon",
               "Required resource not available");
    static QIcon icon(QPixmap(":/Charm/edit.png"));
    return icon;
}

const QIcon& Data::deleteTaskIcon()
{
    Q_ASSERT_X(!QPixmap(":/Charm/deletetask.png").isNull(),
               "Data::deleteTaskIcon", "Required resource not available");
    static QIcon icon(QPixmap(":/Charm/deletetask.png"));
    return icon;
}

const QIcon& Data::editEventIcon()
{
    Q_ASSERT_X(!QPixmap(":/Charm/edit.png").isNull(), "Data::editEventIcon",
               "Required resource not available");
    static QIcon icon(QPixmap(":/Charm/edit.png"));
    return icon;
}

const QIcon& Data::createReportIcon()
{
    Q_ASSERT_X(!QPixmap(":/Charm/createreport.png").isNull(), "Data::createReportIcon",
               "Required resource not available");
    static QIcon icon(QPixmap(":/Charm/createreport.png"));
    return icon;
}

const QPixmap& Data::activePixmap()
{
    static QPixmap pixmap(":/Charm/active.png");
    Q_ASSERT_X(!pixmap.isNull(), "Data::activePixmap",
               "Required resource not available");
    return pixmap;
}

const QIcon& Data::quitCharmIcon()
{
    Q_ASSERT_X(!QPixmap(":/Charm/quitcharm.png").isNull(),
               "Data::quitCharmIcon", "Required resource not available");
    static QIcon icon(QPixmap(":/Charm/quitcharm.png"));
    return icon;
}

const QIcon& Data::configureIcon()
{
    Q_ASSERT_X(!QPixmap(":/Charm/configure.png").isNull(),
               "Data::quitCharmIcon", "Required resource not available");
    static QIcon icon(QPixmap(":/Charm/configure.png"));
    return icon;
}

const QPixmap& Data::editorLockedPixmap()
{
    static QPixmap pixmap(":/Charm/editor_locked.png");
    Q_ASSERT_X(!pixmap.isNull(), "Data::editorLockedPixmap",
               "Required resource not available");
    return pixmap;
}

const QPixmap& Data::editorDirtyPixmap()
{
    static QPixmap pixmap(":/Charm/editor_dirty.png");
    Q_ASSERT_X(!pixmap.isNull(), "Data::editorDirtyPixmap",
               "Required resource not available");
    return pixmap;
}

const QPixmap& Data::backdropPixmap()
{
    static QPixmap pixmap;
    return pixmap;
}
