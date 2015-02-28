/*
  TaskIdDialog.cpp

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

#include "TaskIdDialog.h"

#include <QDialogButtonBox>
#include <QPushButton>

TaskIdDialog::TaskIdDialog( TaskModelInterface* model, TasksView* parent )
    : QDialog( parent )
    , m_model( model )
{
    m_ui.setupUi( this );
    m_ui.spinBox->setRange( 1, 1000*1000*1000 );
    connect( m_ui.buttonBox, SIGNAL(accepted()), this, SLOT(accept()) );
    connect( m_ui.buttonBox, SIGNAL(rejected()), this, SLOT(reject()) );
    // resize( minimumSize() );
}

TaskIdDialog::~TaskIdDialog()
{
}

void TaskIdDialog::setSuggestedId( int id )
{
    m_ui.spinBox->setValue( id );
    m_ui.spinBox->selectAll();
}

void TaskIdDialog::on_spinBox_valueChanged( int value )
{
    const bool taskExists = m_model->taskIdExists( value );
    m_ui.buttonBox->button( QDialogButtonBox::Ok )->setEnabled( !taskExists );
    m_ui.labelExists->setText( taskExists ? tr( "(not ok, exists)" ) : tr( "(ok, does not exist)" ) );
}

int TaskIdDialog::selectedId() const
{
    return m_ui.spinBox->value();
}

QString TaskIdDialog::taskName() const
{
    return m_ui.taskName->text();
}

#include "moc_TaskIdDialog.cpp"
