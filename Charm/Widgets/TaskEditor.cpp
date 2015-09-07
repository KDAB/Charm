/*
  TaskEditor.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2008-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>
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

#include "TaskEditor.h"
#include "SelectTaskDialog.h"
#include "ViewHelpers.h"

#include "Core/CharmConstants.h"
#include "Core/CharmDataModel.h"
#include "Core/TaskTreeItem.h"

#include <QCalendarWidget>
#include <QCheckBox>
#include <QPushButton>
#include <QMessageBox>

#include "ui_TaskEditor.h"

TaskEditor::TaskEditor( QWidget* parent )
    : QDialog( parent )
    , m_ui( new Ui::TaskEditor )
{
    m_ui->setupUi( this );
    m_ui->dateEditFrom->calendarWidget()->setFirstDayOfWeek( Qt::Monday );
    m_ui->dateEditFrom->calendarWidget()->setVerticalHeaderFormat( QCalendarWidget::ISOWeekNumbers );
    m_ui->dateEditTo->calendarWidget()->setFirstDayOfWeek( Qt::Monday );
    m_ui->dateEditTo->calendarWidget()->setVerticalHeaderFormat( QCalendarWidget::ISOWeekNumbers );
    connect( m_ui->pushButtonParent, SIGNAL(clicked()),
             SLOT(slotSelectParent()) );
    connect( m_ui->dateEditFrom, SIGNAL(dateChanged(QDate)),
             SLOT(slotDateChanged(QDate)) );
    connect( m_ui->dateEditTo, SIGNAL(dateChanged(QDate)),
             SLOT(slotDateChanged(QDate)) );
    connect( m_ui->checkBoxFrom, SIGNAL(clicked(bool)),
             SLOT(slotCheckBoxChecked(bool)) );
    connect( m_ui->checkBoxUntil, SIGNAL(clicked(bool)),
             SLOT(slotCheckBoxChecked(bool)) );
}

TaskEditor::~TaskEditor()
{
}

void TaskEditor::setTask( const Task& task )
{
    Q_ASSERT( m_ui );
    m_task = task;
    const TaskTreeItem& taskTreeItem =
    MODEL.charmDataModel()->taskTreeItem( task.id() );
    m_ui->labelTaskName->setText( MODEL.charmDataModel()->fullTaskName( taskTreeItem.task() ) );
    m_ui->lineEditName->setText( task.name() );
    m_ui->checkBoxTrackable->setChecked( task.trackable() );
    if(  task.parent() != 0 ) {
        const TaskTreeItem& parentItem =
                MODEL.charmDataModel()->taskTreeItem( task.parent() );
        const QString name = parentItem.task().name();
        m_ui->pushButtonParent->setText( name );
    } else {
        m_ui->pushButtonParent->setText( tr( "Choose Parent Task" ) );
    }
    if( task.parent() == 0 ) {
        m_ui->checkBoxTopLevel->setChecked( true );
    } else {
        m_ui->checkBoxTopLevel->setChecked( false );
    }
    QDate start = task.validFrom().date();
    if( start.isValid() ) {
        m_ui->dateEditFrom->setDate( start );
        m_ui->checkBoxFrom->setChecked( false );
    } else {
        m_ui->checkBoxFrom->setChecked( true );
        m_ui->dateEditFrom->setDate( QDate::currentDate() );
    }
    QDate end = task.validUntil().date();
    if( end.isValid() ) {
        m_ui->dateEditTo->setDate( end );
        m_ui->checkBoxUntil->setChecked( false );
    } else {
        m_ui->checkBoxUntil->setChecked( true );
        m_ui->dateEditTo->setDate( QDate::currentDate() );
    }
    checkInvariants();
}

Task TaskEditor::getTask() const
{
    Task newTask = m_task;
    newTask.setName( m_ui->lineEditName->text() );
    newTask.setTrackable( m_ui->checkBoxTrackable->isChecked() );
    if( m_ui->checkBoxTopLevel->isChecked() ) {
        newTask.setParent( 0 );
    }
    if( m_ui->checkBoxFrom->isChecked() ) {
        newTask.setValidFrom( QDateTime() );
    } else {
        newTask.setValidFrom( m_ui->dateEditFrom->dateTime() );
    }
    if( m_ui->checkBoxUntil->isChecked() ) {
        newTask.setValidUntil( QDateTime() );
    } else {
        newTask.setValidUntil( m_ui->dateEditTo->dateTime() );
    }
    return newTask;
}

bool parentTreeIsTree( TaskId newParent, TaskId task )
{
    QList<TaskId> parents;
    TaskId parent = newParent;
    while( parent != 0 ) {
        parents << parent;
        const TaskTreeItem& parentItem = MODEL.charmDataModel()->taskTreeItem( parent );
        parent = parentItem.task().parent();
    }
    return ! parents.contains( task );
}

void TaskEditor::slotSelectParent()
{
    SelectTaskDialog dialog( this );
    dialog.setNonTrackableSelectable();
    Q_FOREVER {
        if ( dialog.exec() ) {
            TaskId newParentId = dialog.selectedTask();
            const TaskTreeItem& parentItem =
                MODEL.charmDataModel()->taskTreeItem( newParentId );
            QString name = m_task.name();
            QString parent = parentItem.task().name();
            if( parentTreeIsTree( newParentId, m_task.id() ) ) {
                m_task.setParent( dialog.selectedTask() );
                if( newParentId != 0 ) {
                    m_ui->pushButtonParent->setText( parent );
                } else {
                    m_ui->pushButtonParent->setText( tr( "Choose Parent Task" ) );
                }

                break;
            }
            QMessageBox::information( this, tr( "Please choose another task" ),
                    tr( "The task \"%1\" cannot be selected as the parent task for \"%2\","
                        " because they are the same, or \"%3\" is a direct or indirect subtask of \"%4\".")
                        .arg( parent ).arg( name ).arg( parent ).arg( name ) );
        } else {
            break;
        }
    }
}

void TaskEditor::slotDateChanged( const QDate& )
{
    checkInvariants();
}

void TaskEditor::slotCheckBoxChecked( bool )
{
    checkInvariants();
}

void TaskEditor::checkInvariants()
{
    bool acceptable;
    // the start and end dates are acceptable if
    // * both start and end date are deactivated,
    // * if only one date is set, or
    // * if none are set, and the start date is before the end date
    if( m_ui->checkBoxFrom->isChecked() && m_ui->checkBoxUntil->isChecked() ) {
        acceptable = true;
    } else if ( m_ui->checkBoxFrom->isChecked() || m_ui->checkBoxUntil->isChecked() ) {
        acceptable = true;
    } else if ( m_ui->dateEditFrom->dateTime() < m_ui->dateEditTo->dateTime() ) {
        acceptable = true;
    } else {
        acceptable = false;
    }

    m_ui->buttonBox->button( QDialogButtonBox::Ok )->setEnabled( acceptable );
}

#include "moc_TaskEditor.cpp"
