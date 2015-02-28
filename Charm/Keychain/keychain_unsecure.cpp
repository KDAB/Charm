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
