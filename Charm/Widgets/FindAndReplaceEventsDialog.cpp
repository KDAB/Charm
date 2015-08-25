/*
  FindAndReplaceEventsDialog.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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


#include "FindAndReplaceEventsDialog.h"
#include "ui_FindAndReplaceEventsDialog.h"

#include "ApplicationCore.h"
#include "EventEditorDelegate.h"
#include "SelectTaskDialog.h"
#include "ViewHelpers.h"

#include "Commands/CommandModifyEvent.h"

#include <QCalendarWidget>
#include <QMessageBox>

FindAndReplaceEventsDialog::FindAndReplaceEventsDialog( QWidget* parent ) :
    QDialog( parent )
   ,m_taskToSearch( 0 )
   ,m_taskToReplaceWith( 0 )
   ,m_ui( new Ui::FindAndReplaceEventsDialog )
{
    m_ui->setupUi(this);

    m_replace = new QPushButton( tr( "Replace" ) );
    m_replace->setEnabled( false );
    connect( m_replace, SIGNAL(clicked()), SLOT(slotReplaceProjectCode()) );
    m_cancel = new QPushButton( tr( "Cancel") );
    connect( m_cancel, SIGNAL(clicked()), SLOT(reject()) );
    m_ui->buttonBox->addButton( m_cancel, QDialogButtonBox::RejectRole );
    m_ui->buttonBox->addButton( m_replace, QDialogButtonBox::AcceptRole );

    m_ui->dateEditEnd->calendarWidget()->setFirstDayOfWeek( Qt::Monday );
    m_ui->dateEditEnd->calendarWidget()->setVerticalHeaderFormat( QCalendarWidget::ISOWeekNumbers );
    m_ui->dateEditStart->calendarWidget()->setFirstDayOfWeek( Qt::Monday );
    m_ui->dateEditStart->calendarWidget()->setVerticalHeaderFormat( QCalendarWidget::ISOWeekNumbers );

    const TimeSpans timeSpans;
    m_timeSpan = timeSpans.thisWeek().timespan;
    m_ui->dateEditStart->setDate( m_timeSpan.first );
    // less 1 day as the timespan logic in charm excludes events on the end date
    // and we add a day when filtering.
    m_ui->dateEditEnd->setDate( m_timeSpan.second.addDays( -1 ) );

    connect( m_ui->dateEditStart->calendarWidget(), SIGNAL(selectionChanged()), SLOT(slotTimeSpansChanged()) );
    connect( m_ui->dateEditEnd->calendarWidget(), SIGNAL(selectionChanged()), SLOT(slotTimeSpansChanged()) );
    connect( ApplicationCore::instance().dateChangeWatcher(),
             SIGNAL(dateChanged()),
             SLOT(slotTimeSpansChanged()) );

    m_ui->selectReplaceWithTaskPB->setEnabled( false );
    connect ( m_ui->selectSearchTaskPB, SIGNAL(clicked()), SLOT(slotSelectTaskToSearch()) );
    connect ( m_ui->selectReplaceWithTaskPB, SIGNAL(clicked()), SLOT(slotSelectTaskToReplaceWith()) );

    m_model.reset( new EventModelFilter( DATAMODEL ) );
    m_ui->findAndReplaceLV->setModel( m_model.data() );
    m_model->setFilterStartDate( m_timeSpan.first );
    m_model->setFilterEndDate( m_timeSpan.second );

    auto delegate = new EventEditorDelegate( m_model.data(), m_ui->findAndReplaceLV );
    m_ui->findAndReplaceLV->setItemDelegate( delegate );
}

FindAndReplaceEventsDialog::~FindAndReplaceEventsDialog()
{

}

void FindAndReplaceEventsDialog::searchProjectCode()
{
    m_foundEvents = m_model->events();

    if ( m_foundEvents.count() > 0 )
        m_ui->selectReplaceWithTaskPB->setEnabled( true );
}

void FindAndReplaceEventsDialog::slotReplaceProjectCode()
{
    const QList<Event> events = m_model->events();

    for ( int i = 0; i < events.count(); ++i ) {
        if ( m_foundEvents.contains( events[i] ) ) {
            Event event = events[i];
            event.setTaskId( m_taskToReplaceWith );
            m_modifiedEvents << event;
        }
    }

    accept();
}

QList<Event> FindAndReplaceEventsDialog::modifiedEvents() const
{
    return m_modifiedEvents;
}

void FindAndReplaceEventsDialog::slotTimeSpansChanged()
{
    m_timeSpan.first = m_ui->dateEditStart->date();
    m_timeSpan.second = m_ui->dateEditEnd->date();
    m_model->setFilterStartDate( m_timeSpan.first );
    // add a day as the timespan logic in charm excludes events on the end date.
    m_model->setFilterEndDate( m_timeSpan.second.addDays( 1 ) );
    if ( m_taskToSearch > 0 )
        searchProjectCode();
}

void FindAndReplaceEventsDialog::slotSelectTaskToSearch()
{
   selectTask( TaskToSearch );
}

void FindAndReplaceEventsDialog::slotSelectTaskToReplaceWith()
{
   selectTask( TaskToReplaceWith );
}

void FindAndReplaceEventsDialog::selectTask( SelectTaskType type )
{
    SelectTaskDialog dialog( this );
    if( !dialog.exec() )
        return;

    const int taskId = dialog.selectedTask();
    if ( type == TaskToSearch ) {
        m_taskToSearch = taskId;
        m_ui->findTaskLB->setText( DATAMODEL->taskIdAndSmartNameString( taskId ) );
        m_model->setFilterTaskId( taskId );
    } else {
        m_taskToReplaceWith = taskId;
        m_ui->replaceWithTaskLB->setText( DATAMODEL->taskIdAndSmartNameString( taskId ) );
    }

    m_replace->setEnabled( m_taskToReplaceWith > 0 && m_taskToSearch > 0 );

    searchProjectCode();
}

#include "moc_FindAndReplaceEventsDialog.cpp"
