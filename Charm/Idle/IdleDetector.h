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
 * detection is not supported, a null pointer is returned.
 */
class IdleDetector : public QObject
{
    Q_OBJECT

public:
    typedef QPair<QDateTime, QDateTime> IdlePeriod;
    typedef QVector<IdlePeriod> IdlePeriods;

    /** Create an idle detector for this platform.
     * Returns 0 if idle detection is not available. */
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

protected:
    virtual void idlenessDurationChanged() {}
    explicit IdleDetector( QObject* parent = nullptr );
    virtual ~IdleDetector() {}
    void maybeIdle( IdlePeriod period );

Q_SIGNALS:
    void maybeIdle();

private:
    IdlePeriods m_idlePeriods;
    int m_idlenessDuration;
};

#endif
