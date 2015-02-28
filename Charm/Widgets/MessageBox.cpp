/*
  MessageBox.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Frank Osterfeld <frank.osterfeld@kdab.com>

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

#include "MessageBox.h"

#include <QAbstractButton>
#include <QPointer>

static int messageBox( QWidget* parent,
                       const QString& title,
                       const QString& text,
                       QMessageBox::StandardButton yesButton,
                       const QString& yesLabel,
                       QMessageBox::StandardButton noButton,
                       const QString& noLabel,
                       QMessageBox::StandardButton defaultButton,
                       QMessageBox::Icon icon )
{
    QPointer<QMessageBox> messageBox( new QMessageBox( parent ) );
    messageBox->setWindowTitle( title );
    messageBox->setIcon( icon );
    messageBox->setText( text );
    messageBox->setStandardButtons( yesButton|noButton );
    messageBox->button( yesButton )->setText( yesLabel );
    messageBox->button( noButton )->setText( noLabel );
    messageBox->setDefaultButton( defaultButton );
    const int result = messageBox->exec();
    delete messageBox;
    return result;
}

int MessageBox::question( QWidget* parent,
    const QString& title,
    const QString& text,
    const QString& yesLabel,
    const QString& noLabel,
    QMessageBox::StandardButton defaultButton )
{
    return messageBox( parent, title, text, QMessageBox::Yes, yesLabel, QMessageBox::No, noLabel, defaultButton, QMessageBox::Question );
}

int MessageBox::warning( QWidget* parent,
    const QString& title,
    const QString& text,
    const QString& yesLabel,
    const QString& noLabel,
    QMessageBox::StandardButton defaultButton )
{
    return messageBox( parent, title, text, QMessageBox::Yes, yesLabel, QMessageBox::No, noLabel, defaultButton, QMessageBox::Warning );
}
