#ifndef MACCOCOAIDLEDETECTOR_H
#define MACCOCOAIDLEDETECTOR_H

#include <Cocoa/Cocoa.h>

#include <QObject>
#include <QDateTime>

#include "IdleDetector.h"

class MacCocoaIdleDetector : public IdleDetector
{
    Q_OBJECT

public:
    explicit MacCocoaIdleDetector( QObject* parent = 0 );
    void idle();

private:
    id m_observer;
};

#endif
