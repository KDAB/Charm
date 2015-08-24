#ifndef NOTIFICATIONPOPUP_H
#define NOTIFICATIONPOPUP_H

#include <QDialog>
#include <QScopedPointer>

namespace Ui {
    class NotificationPopup;
}

class NotificationPopup : public QDialog
{
    Q_OBJECT

public:
    explicit NotificationPopup( QWidget *parent = nullptr );
    ~NotificationPopup();

    void showNotification( const QString& title, const QString& message );

private slots:
    void slotCloseNotification();

private:
    void mousePressEvent( QMouseEvent* event ) override;

    QScopedPointer<Ui::NotificationPopup> m_ui;
};

#endif // NOTIFICATIONPOPUP_H

