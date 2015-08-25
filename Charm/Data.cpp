/*
  Data.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2007-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>
  Author: Frank Osterfeld <frank.osterfeld@kdab.com>
  Author: Mike McQuaid <mike.mcquaid@kdab.com>
  Author: David Faure <david.faure@kdab.com>
  Author: Mike Arthur <mike.mcquaid@kdab.com>
  Author: Allen Winter <allen.winter@kdab.com>

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

#include "Data.h"

#include <QIcon>
#include <QPixmap>

const QIcon& Data::charmIcon()
{
    Q_ASSERT_X(!QPixmap(":/Charm/charmicon.png").isNull(), Q_FUNC_INFO,
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

#ifdef Q_OS_OSX
    static const QString iconPath = QLatin1String(":/Charm/charmtray_mac.png");
#else
    static const QString iconPath = QLatin1String(":/Charm/charmtray22.png");
#endif

    Q_ASSERT_X(!QPixmap(iconPath).isNull(), Q_FUNC_INFO,
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

#ifdef Q_OS_OSX
    static const QString iconPath = QLatin1String(":/Charm/charmtrayactive_mac.png");
#else
    static const QString iconPath = QLatin1String(":/Charm/charmtrayactive22.png");
#endif

    Q_ASSERT_X(!QPixmap(iconPath).isNull(), Q_FUNC_INFO,
                        "Required resource not available");
    static QIcon icon = QPixmap(iconPath);
    return icon;
}
const QIcon& Data::goIcon()
{
    Q_ASSERT_X(!QPixmap(":/Charm/go.png").isNull(), Q_FUNC_INFO,
              "Required resource not available");
    static QIcon icon(QPixmap(":/Charm/go.png"));
    return icon;
}

const QIcon& Data::stopIcon()
{
    Q_ASSERT_X(!QPixmap(":/Charm/stop.png").isNull(), Q_FUNC_INFO,
            "Required resource not available");
    static QIcon icon(QPixmap(":/Charm/stop.png"));
    return icon;
}

const QIcon& Data::newTaskIcon()
{
    Q_ASSERT_X(!QPixmap(":/Charm/newtask.png").isNull(), Q_FUNC_INFO,
               "Required resource not available");
    static QIcon icon(QPixmap(":/Charm/newtask.png"));
    return icon;
}

const QIcon& Data::newSubtaskIcon()
{
    Q_ASSERT_X(!QPixmap(":/Charm/newsubtask.png").isNull(), Q_FUNC_INFO,
               "Required resource not available");
    static QIcon icon(QPixmap(":/Charm/newsubtask.png"));
    return icon;
}

const QIcon& Data::editTaskIcon()
{   // FIXME same as edit-event icon
    Q_ASSERT_X(!QPixmap(":/Charm/edit.png").isNull(), Q_FUNC_INFO,
               "Required resource not available");
    static QIcon icon(QPixmap(":/Charm/edit.png"));
    return icon;
}

const QIcon& Data::deleteTaskIcon()
{
    Q_ASSERT_X(!QPixmap(":/Charm/deletetask.png").isNull(),
               Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/Charm/deletetask.png"));
    return icon;
}

const QIcon& Data::searchIcon()
{
    Q_ASSERT_X(!QPixmap(":/Charm/search.png").isNull(),
               Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/Charm/search.png"));
    return icon;
}

const QIcon& Data::editEventIcon()
{
    Q_ASSERT_X(!QPixmap(":/Charm/edit.png").isNull(), Q_FUNC_INFO,
               "Required resource not available");
    static QIcon icon(QPixmap(":/Charm/edit.png"));
    return icon;
}

const QIcon& Data::createReportIcon()
{
    Q_ASSERT_X(!QPixmap(":/Charm/createreport.png").isNull(), Q_FUNC_INFO,
               "Required resource not available");
    static QIcon icon(QPixmap(":/Charm/createreport.png"));
    return icon;
}

const QPixmap& Data::activePixmap()
{
    static QPixmap pixmap(":/Charm/active.png");
    Q_ASSERT_X(!pixmap.isNull(), Q_FUNC_INFO,
               "Required resource not available");
    return pixmap;
}

const QIcon& Data::quitCharmIcon()
{
    Q_ASSERT_X(!QPixmap(":/Charm/quitcharm.png").isNull(),
               Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/Charm/quitcharm.png"));
    return icon;
}

const QIcon& Data::configureIcon()
{
    Q_ASSERT_X(!QPixmap(":/Charm/configure.png").isNull(),
               Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/Charm/configure.png"));
    return icon;
}

const QPixmap& Data::editorLockedPixmap()
{
    static QPixmap pixmap(":/Charm/editor_locked.png");
    Q_ASSERT_X(!pixmap.isNull(), Q_FUNC_INFO,
               "Required resource not available");
    return pixmap;
}

const QPixmap& Data::editorDirtyPixmap()
{
    static QPixmap pixmap(":/Charm/editor_dirty.png");
    Q_ASSERT_X(!pixmap.isNull(), Q_FUNC_INFO,
               "Required resource not available");
    return pixmap;
}
