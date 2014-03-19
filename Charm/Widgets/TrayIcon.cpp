#include "TrayIcon.h"

#include "ApplicationCore.h"

#include <QEvent>

TrayIcon::TrayIcon(QObject* parent)
{
    connect(this,
            SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            SLOT(slotActivated(QSystemTrayIcon::ActivationReason)));
}

TrayIcon::~TrayIcon()
{

}

void TrayIcon::slotActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
    case QSystemTrayIcon::Context:
        // show context menu
        // m_systrayContextMenu.show();
        break;
    case QSystemTrayIcon::Trigger: //(single click)
    case QSystemTrayIcon::DoubleClick:
#ifndef Q_OS_OSX
        ApplicationCore::instance().toggleShowHide();
#endif
        break;
    case QSystemTrayIcon::MiddleClick:
        // TODO: Start task?
        ApplicationCore::instance().slotStopAllTasks();
        break;
    case QSystemTrayIcon::Unknown:
    default:
        break;
    }
}

#include "moc_TrayIcon.cpp"
