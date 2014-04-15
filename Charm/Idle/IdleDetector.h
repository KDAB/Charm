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
    explicit IdleDetector( QObject* parent = 0 );
    void maybeIdle( IdlePeriod period );
    void setAvailable( bool available );

private:
    IdlePeriods m_idlePeriods;
    int m_idlenessDuration;
    bool m_available;
};

#endif
