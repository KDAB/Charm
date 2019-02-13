/*
  Data.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2007-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

const QIcon &Data::charmIcon()
{
    static QIcon icon(QStringLiteral(":/Charm/charmicon.png"));
    Q_ASSERT_X(!icon.isNull(), Q_FUNC_INFO,
               "Required resource not available");
    return icon;
}

const QIcon &Data::charmTrayIcon()
{
    // Mac and Windows: the system tray uses 16x16.
    // TODO: different icons though: white background on Windows
    // On X11: pure-Qt apps get 22x22 from QSystemTrayIcon.
    // KDE apps seem to get 24x24 in KSystemTrayIcon via KIconLoader, which is actually better.

#ifdef Q_OS_OSX
    static const QString iconPath = QStringLiteral(":/Charm/charmtray_mac.png");
#else
    static const QString iconPath = QStringLiteral(":/Charm/charmtray22.png");
#endif

    Q_ASSERT_X(!QPixmap(iconPath).isNull(), Q_FUNC_INFO,
               "Required resource not available");
    static QIcon icon = QPixmap(iconPath);
    return icon;
}

const QIcon &Data::charmTrayActiveIcon()
{
    // Mac and Windows: the system tray uses 16x16.
    // TODO: different icons though: white background on Windows
    // On X11: pure-Qt apps get 22x22 from QSystemTrayIcon.
    // KDE apps seem to get 24x24 in KSystemTrayIcon via KIconLoader, which is actually better.

#ifdef Q_OS_OSX
    static const QString iconPath = QStringLiteral(":/Charm/charmtrayactive_mac.png");
#else
    static const QString iconPath = QStringLiteral(":/Charm/charmtrayactive22.png");
#endif

    Q_ASSERT_X(!QPixmap(iconPath).isNull(), Q_FUNC_INFO,
               "Required resource not available");
    static QIcon icon = QPixmap(iconPath);
    return icon;
}

const QIcon &Data::goIcon()
{
    static QIcon icon(QStringLiteral(":/Charm/go.png"));
    Q_ASSERT_X(!icon.isNull(), Q_FUNC_INFO,
               "Required resource not available");
    return icon;
}

const QIcon &Data::stopIcon()
{
    static QIcon icon(QStringLiteral(":/Charm/stop.png"));
    Q_ASSERT_X(!icon.isNull(), Q_FUNC_INFO,
               "Required resource not available");
    return icon;
}

const QIcon &Data::newTaskIcon()
{
    static QIcon icon(QStringLiteral(":/Charm/newtask.png"));
    Q_ASSERT_X(!icon.isNull(), Q_FUNC_INFO,
               "Required resource not available");
    return icon;
}

const QIcon &Data::newSubtaskIcon()
{
    static QIcon icon(QStringLiteral(":/Charm/newsubtask.png"));
    Q_ASSERT_X(!icon.isNull(), Q_FUNC_INFO,
               "Required resource not available");
    return icon;
}

const QIcon &Data::editTaskIcon()
{   // FIXME same as edit-event icon
    static QIcon icon(QStringLiteral(":/Charm/edit.png"));
    Q_ASSERT_X(!icon.isNull(), Q_FUNC_INFO,
               "Required resource not available");
    return icon;
}

const QIcon &Data::deleteTaskIcon()
{
    static QIcon icon(QStringLiteral(":/Charm/deletetask.png"));
    Q_ASSERT_X(!icon.isNull(),
               Q_FUNC_INFO, "Required resource not available");
    return icon;
}

const QIcon &Data::searchIcon()
{
    static QIcon icon(QStringLiteral(":/Charm/search.png"));
    Q_ASSERT_X(!icon.isNull(),
               Q_FUNC_INFO, "Required resource not available");
    return icon;
}

const QIcon &Data::editEventIcon()
{
    static QIcon icon(QStringLiteral(":/Charm/edit.png"));
    Q_ASSERT_X(!icon.isNull(), Q_FUNC_INFO,
               "Required resource not available");
    return icon;
}

const QIcon &Data::createReportIcon()
{
    static QIcon icon(QStringLiteral(":/Charm/createreport.png"));
    Q_ASSERT_X(!icon.isNull(), Q_FUNC_INFO,
               "Required resource not available");
    return icon;
}

const QPixmap &Data::activePixmap()
{
    static QPixmap pixmap(QStringLiteral(":/Charm/active.png"));
    Q_ASSERT_X(!pixmap.isNull(), Q_FUNC_INFO,
               "Required resource not available");
    return pixmap;
}

const QIcon &Data::quitCharmIcon()
{
    static QIcon icon(QStringLiteral(":/Charm/quitcharm.png"));
    Q_ASSERT_X(!icon.isNull(),
               Q_FUNC_INFO, "Required resource not available");
    return icon;
}

const QIcon &Data::configureIcon()
{
    static QIcon icon(QStringLiteral(":/Charm/configure.png"));
    Q_ASSERT_X(!icon.isNull(),
               Q_FUNC_INFO, "Required resource not available");
    return icon;
}

const QPixmap &Data::editorLockedPixmap()
{
    static QPixmap pixmap(QStringLiteral(":/Charm/editor_locked.png"));
    Q_ASSERT_X(!pixmap.isNull(), Q_FUNC_INFO,
               "Required resource not available");
    return pixmap;
}

const QPixmap &Data::editorDirtyPixmap()
{
    static QPixmap pixmap(QStringLiteral(":/Charm/editor_dirty.png"));
    Q_ASSERT_X(!pixmap.isNull(), Q_FUNC_INFO,
               "Required resource not available");
    return pixmap;
}
