#ifndef X11IDLEDETECTOR_H
#define X11IDLEDETECTOR_H

#include "IdleDetector.h"

class X11IdleDetector : public IdleDetector
{
    Q_OBJECT
public:
    explicit X11IdleDetector( QObject* parent );
    static bool idleCheckPossible();

private slots:
    void checkIdleness();

private:
    QDateTime m_heartbeat;
};


#endif /* X11IDLEDETECTOR_H */

