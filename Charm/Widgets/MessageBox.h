/*
  MessageBox.h

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

#ifndef CHARM_MESSAGEBOX_H
#define CHARM_MESSAGEBOX_H

#include <QMessageBox>

namespace MessageBox
{
int question( QWidget* parent,
              const QString& title,
              const QString& text,
              const QString& yesLabel,
              const QString& noLabel,
              QMessageBox::StandardButton defaultButton=QMessageBox::NoButton );

int warning( QWidget* parent,
              const QString& title,
              const QString& text,
              const QString& yesLabel,
              const QString& noLabel,
              QMessageBox::StandardButton defaultButton=QMessageBox::NoButton );

}
#endif
