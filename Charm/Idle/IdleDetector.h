#ifndef IDLEDETECTOR_H
#define IDLEDETECTOR_H

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
    /** Create an idle detector for this platform.
     * Returns 0 if idle detection is not available. */
    static IdleDetector* createIdleDetector( QObject* parent );

protected:
    explicit IdleDetector( QObject* parent = 0 );
    virtual ~IdleDetector() {}

Q_SIGNALS:
    void maybeIdle( QDateTime );
};

#endif
