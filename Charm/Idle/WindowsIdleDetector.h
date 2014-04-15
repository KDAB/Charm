#ifndef WINDOWSIDLEDETECTOR_H
#define WINDOWSIDLEDETECTOR_H

#include "IdleDetector.h"

#include <QTimer>

class WindowsIdleDetector : public IdleDetector {
    Q_OBJECT
public:
    explicit WindowsIdleDetector( QObject* parent );

protected:
    void onIdlenessDurationChanged();

private Q_SLOTS:
    void timeout();

private:
    QTimer m_timer;
};

#endif // WINDOWSIDLEDETECTOR_H
