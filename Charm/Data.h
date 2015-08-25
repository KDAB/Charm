/*
  Data.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2007-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

#ifndef DATA_H
#define DATA_H

class QIcon;
class QPixmap;

class Data {
public:
    static const QIcon& charmIcon();
    static const QIcon& charmTrayIcon();
    static const QIcon& charmTrayActiveIcon();

    static const QIcon& goIcon();
    static const QIcon& stopIcon();
    static const QIcon& newTaskIcon();
    static const QIcon& newSubtaskIcon();
    static const QIcon& editTaskIcon();
    static const QIcon& deleteTaskIcon();
    static const QIcon& editEventIcon();
    static const QIcon& searchIcon();
    static const QIcon& previousEventIcon();
    static const QIcon& nextEventIcon();
    static const QIcon& createReportIcon();
    static const QPixmap& checkIcon();
    static const QPixmap& activePixmap();
    static const QIcon& quitCharmIcon();
    static const QIcon& clearFilterIcon();
    static const QIcon& configureIcon();
    static const QPixmap& editorLockedPixmap();
    static const QPixmap& editorDirtyPixmap();
    static const QPixmap& recorderStopIcon();
    static const QPixmap& recorderGoIcon();
};

#endif
