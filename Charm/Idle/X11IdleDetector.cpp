/*
  X11IdleDetector.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2008-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Jesper Pedersen <jesper.pedersen@kdab.com>
  Author: Frank Osterfeld <frank.osterfeld@kdab.com>
  Author: Mirko Boehm <mirko.boehm@kdab.com>
  Author: Mike McQuaid <mike.mcquaid@kdab.com>

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

#include "X11IdleDetector.h"
#include "CharmCMake.h"

#include <xcb/screensaver.h>

X11IdleDetector::X11IdleDetector(QObject *parent)
    : IdleDetector(parent)
{
    connect(&m_timer, &QTimer::timeout, this, &X11IdleDetector::checkIdleness);
    m_timer.start(idlenessDuration() * 1000 / 5);
    m_heartbeat = QDateTime::currentDateTime();
}

bool X11IdleDetector::idleCheckPossible()
{
    m_connection = xcb_connect(NULL, NULL); //krazy:exclude=null
    m_screen = xcb_setup_roots_iterator(xcb_get_setup(m_connection)).data;
    if (m_screen)
        return true;
    return false;
}

void X11IdleDetector::onIdlenessDurationChanged()
{
    m_timer.stop();
    m_timer.start(idlenessDuration() * 1000 / 5);
}

void X11IdleDetector::checkIdleness()
{
    xcb_screensaver_query_info_cookie_t cookie;
    cookie = xcb_screensaver_query_info(m_connection, m_screen->root);
    xcb_screensaver_query_info_reply_t *info;
    info = xcb_screensaver_query_info_reply(m_connection, cookie, NULL);    //krazy:exclude=null
    const int idleSecs = info->ms_since_user_input / 1000;
    free(info);

    if (idleSecs >= idlenessDuration())
        maybeIdle(IdlePeriod(QDateTime::currentDateTime().addSecs(-idleSecs),
                             QDateTime::currentDateTime()));

    if (m_heartbeat.secsTo(QDateTime::currentDateTime()) > idlenessDuration())
        maybeIdle(IdlePeriod(m_heartbeat, QDateTime::currentDateTime()));

    m_heartbeat = QDateTime::currentDateTime();
}

#include "moc_X11IdleDetector.cpp"
