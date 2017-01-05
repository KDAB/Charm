/*
  MacIdleDetector.mm

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2011-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

#include <Cocoa/Cocoa.h>

#include "MacIdleDetector.h"

#include <QDateTime>

@interface MacIdleObserver : NSObject
{
@public
    MacIdleDetector* idleDetector;
    QDateTime idleStartTime;
}
- (id)init;
- (void)dealloc;
@end

@implementation MacIdleObserver
- (id)init {
    if ((self = [super init])) {
        NSNotificationCenter* notificationCenter = [[NSWorkspace sharedWorkspace] notificationCenter];
        [notificationCenter
         addObserver: self
         selector: @selector(receiveSleepNotification:)
         name: NSWorkspaceWillSleepNotification
         object: NULL ];
        [notificationCenter
         addObserver: self
         selector: @selector(receiveWakeNotification:)
         name: NSWorkspaceDidWakeNotification
         object: NULL ];
        [notificationCenter
         addObserver: self
         selector: @selector(receiveSleepNotification:)
         name: NSWorkspaceScreensDidSleepNotification
         object: NULL ];
        [notificationCenter
         addObserver: self
         selector: @selector(receiveWakeNotification:)
         name: NSWorkspaceScreensDidWakeNotification
         object: NULL ];
    }
    return self;
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [super dealloc];
}

- (void)receiveSleepNotification:(NSNotification*)notification {
    idleStartTime = QDateTime::currentDateTime();
}

- (void)receiveWakeNotification:(NSNotification*)notification {
    if (idleDetector)
        idleDetector->idle();
}
@end

class MacIdleDetector::Private {
public:
    Private();
    ~Private();
    NSAutoreleasePool* pool;
    MacIdleObserver* observer;
};

MacIdleDetector::Private::Private()
    : pool( 0 ), observer( 0 )
{
    pool = [[NSAutoreleasePool alloc] init];
    observer = [[MacIdleObserver alloc] init];
}

MacIdleDetector::Private::~Private()
{
    [pool drain];
}

MacIdleDetector::MacIdleDetector( QObject* parent )
    : IdleDetector( parent )
    , m_private( new MacIdleDetector::Private() )
{
    m_private->observer->idleDetector = this;
}

void MacIdleDetector::idle()
{
    maybeIdle( IdlePeriod( m_private->observer->idleStartTime, QDateTime::currentDateTime() ) );
}

#include "MacIdleDetector.moc"
