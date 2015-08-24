#include "NotificationPopup.h"
#include "ui_NotificationPopup.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QTimer>

NotificationPopup::NotificationPopup( QWidget* parent )
    : QDialog( parent )
    ,m_ui( new Ui::NotificationPopup )
{
    m_ui->setupUi( this );

    setAttribute( Qt::WA_ShowWithoutActivating );
    setWindowFlags( Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint );
}

NotificationPopup::~NotificationPopup()
{
}

void NotificationPopup::showNotification( const QString& title, const QString& message )
{
    QString titleText = m_ui->titleLB->text();
    m_ui->titleLB->setText( titleText.replace( "TITLE", title ) );
    QString messageText = m_ui->messageLB->text();
    m_ui->messageLB->setText( messageText.replace( "MESSAGE", message ) );

    setGeometry( QStyle::alignedRect ( Qt::RightToLeft, Qt::AlignBottom, size(), qApp->desktop()->availableGeometry() ) );
    show();
    QTimer::singleShot(10000, this, SLOT(slotCloseNotification()));
}

void NotificationPopup::slotCloseNotification()
{
    close();
}

void NotificationPopup::mousePressEvent( QMouseEvent* )
{
    close();
}

#include "moc_NotificationPopup.cpp"
