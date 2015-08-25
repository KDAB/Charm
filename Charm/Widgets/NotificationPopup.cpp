/*
  NotificationPopup.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2015-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Michel Boyer de la Giroday <michel.giroday@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
