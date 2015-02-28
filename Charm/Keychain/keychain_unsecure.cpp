/*
  keychain_unsecure.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

/******************************************************************************
 *   Copyright (C) 2011 Frank Osterfeld <frank.osterfeld@gmail.com>           *
 *                                                                            *
 * This program is distributed in the hope that it will be useful, but        *
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY *
 * or FITNESS FOR A PARTICULAR PURPOSE. For licensing and distribution        *
 * details, check the accompanying file 'COPYING'.                            *
 *****************************************************************************/

#include "keychain_p.h"

#include <QSettings>
#include <QScopedPointer>

using namespace QKeychain;

void ReadPasswordJobPrivate::scheduledStart() {
    QScopedPointer<QSettings> local( !q->settings() ? new QSettings( q->service() ) : 0 );
    QSettings* actual = q->settings() ? q->settings() : local.data();
    data = actual->value( key ).toByteArray();
    q->emitFinished();
}

void WritePasswordJobPrivate::scheduledStart() {
    if ( mode == Delete ) {
        QScopedPointer<QSettings> local( !q->settings() ? new QSettings( q->service() ) : 0 );
        QSettings* actual = q->settings() ? q->settings() : local.data();
        actual->remove( key );
        actual->sync();
        q->emitFinished();
    } else {
        QScopedPointer<QSettings> local( !q->settings() ? new QSettings( q->service() ) : 0 );
        QSettings* actual = q->settings() ? q->settings() : local.data();
        QByteArray data = mode == Binary ? binaryData : textData.toUtf8();
        actual->setValue( key, data );
        actual->sync();
        q->emitFinished();
    }
}
