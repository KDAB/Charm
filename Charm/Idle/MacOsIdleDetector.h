#ifndef MACOSIDLEDETECTOR_H
#define MACOSIDLEDETECTOR_H

#include <QObject>

#include "IdleDetector.h"

class MacOsIdleDetector : public IdleDetector
{
    Q_OBJECT

public:
    explicit MacOsIdleDetector( QObject* parent = 0 );
    ~MacOsIdleDetector();


private:
    void idle();

    // The class cannot be ifdefed, since moc will ignore it then. Use
    // a private implementation to keep the Apple API out of the
    // header (the rest is not critical, since the code is never
    // referenced).
    class Private;
    friend class Private;
    Private* d;
};

#endif
