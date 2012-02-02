#ifndef MACCOCOAIDLEDETECTOR_H
#define MACCOCOAIDLEDETECTOR_H

#include <QObject>
#include <QDateTime>

#include "IdleDetector.h"

class MacIdleDetector : public IdleDetector
{
    Q_OBJECT

public:
    explicit MacIdleDetector( QObject* parent = 0 );
    // This method to be public due to lack of friend classes in Objective-C and
    // the lack inheritance of Objective-C classes from C++ ones.
    void idle();

private:
    class Private;
    Private* m_private;
};

#endif
