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

