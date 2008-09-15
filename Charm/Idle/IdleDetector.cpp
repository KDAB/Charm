#include "IdleDetector.h"

IdleDetector::IdleDetector( QObject* parent )
    : QObject( parent )
{
}

IdleDetector* IdleDetector::createIdleDetector( QObject* parent )
{
    return 0;
}

