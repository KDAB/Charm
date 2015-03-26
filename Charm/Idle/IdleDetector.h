/*
  IdleDetector.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2008-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>
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

#ifndef IDLEDETECTOR_H
#define IDLEDETECTOR_H

#include <QPair>
#include <QVector>
#include <QObject>
#include <QDateTime>

/** IdleDetector implements idle detection (duh).
 * Idle detection is (as of now) platform dependant. The factory
 * function createIdleDetector returns an implementation that
 * implements idle detection for the current platform. If idle
 * detection is not supported, a dummy object is returned,
 * with @c available being false.
 */
class IdleDetector : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool available READ available NOTIFY availableChanged)
    Q_PROPERTY(int idlenessDuration READ idlenessDuration WRITE setIdlenessDuration NOTIFY idlenessDurationChanged)

public:
    typedef QPair<QDateTime, QDateTime> IdlePeriod;
    typedef QVector<IdlePeriod> IdlePeriods;

    /** Create an idle detector for this platform. */
    static IdleDetector* createIdleDetector( QObject* parent );

    /** Returns the idle periods. */
    IdlePeriods idlePeriods() const;

    /** Clear the recorded idle periods. */
    void clear();

    /**
     * the number of seconds after which the detector should notify idleness
     * @return the duration in seconds
     */
    int idlenessDuration() const;
    void setIdlenessDuration( int seconds );

    /**
     * Returns whether idle detection is available
     */
    bool available() const;


Q_SIGNALS:
    void maybeIdle();
    void idlenessDurationChanged( int idlenessDuration );
    void availableChanged( bool available );

protected:
    virtual void onIdlenessDurationChanged() {}
    explicit IdleDetector( QObject* parent = nullptr );
    void maybeIdle( IdlePeriod period );
    void setAvailable( bool available );

private:
    IdlePeriods m_idlePeriods;
    int m_idlenessDuration;
    bool m_available;
};

#endif
