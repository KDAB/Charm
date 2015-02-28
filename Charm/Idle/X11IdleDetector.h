/*
  X11IdleDetector.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2008-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Jesper Pedersen <jesper.pedersen@kdab.com>
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

#ifndef X11IDLEDETECTOR_H
#define X11IDLEDETECTOR_H

#include "IdleDetector.h"

#include <QTimer>

class X11IdleDetector : public IdleDetector
{
    Q_OBJECT
public:
    explicit X11IdleDetector( QObject* parent );
    static bool idleCheckPossible();

protected:
    void onIdlenessDurationChanged();

private slots:
    void checkIdleness();

private:
    QDateTime m_heartbeat;
    QTimer m_timer;
};


#endif /* X11IDLEDETECTOR_H */

