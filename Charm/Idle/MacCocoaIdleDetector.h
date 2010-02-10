#ifndef MACCOCOAIDLEDETECTOR_H
#define MACCOCOAIDLEDETECTOR_H

#include <QObject>
#include <QDateTime>

#include "IdleDetector.h"

class objc_object;

class MacCocoaIdleDetector : public IdleDetector
{
    Q_OBJECT

public:
    explicit MacCocoaIdleDetector( QObject* parent = 0 );
    void idle();

private:
    objc_object* m_observer;
};

#endif
