/*
  IdleCorrectionDialog.cpp

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

#include "IdleCorrectionDialog.h"
#include "ui_IdleCorrectionDialog.h"

IdleCorrectionDialog::IdleCorrectionDialog( QWidget* parent )
    : QDialog( parent )
    , m_ui( new Ui::IdleCorrectionDialog )
{
    m_ui->setupUi( this );
}

IdleCorrectionDialog::~IdleCorrectionDialog()
{
}

IdleCorrectionDialog::Result IdleCorrectionDialog::result() const
{
    if ( m_ui->ignore->isChecked() ) {
        return Idle_Ignore;
    } else if ( m_ui->endEvent->isChecked() ) {
        return Idle_EndEvent;
    } else {
        Q_ASSERT( false ); // unhandled whatever?
    }

    return Idle_NoResult;
}

#include "moc_IdleCorrectionDialog.cpp"
