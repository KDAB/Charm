/*
 * EventDisplay.cpp
 *
 *  Created on: Jun 28, 2008
 *      Author: mirko
 */
#include "Core/CharmConstants.h"
#include "Core/TaskTreeItem.h"
#include "Core/CharmDataModel.h"

#include "EventDisplay.h"
#include "ui_EventDisplay.h"
#include "ViewHelpers.h"
#include "Reports/CharmReport.h"

EventDisplay::EventDisplay( QWidget* parent )
	: QWidget( parent )
{
	m_ui = new Ui::EventDisplay();
	m_ui->setupUi( this );
}

EventDisplay::~EventDisplay()
{
}

void EventDisplay::mouseDoubleClickEvent ( QMouseEvent* )
{
	emit editEvent( m_event );
}

void EventDisplay::setEvent( const Event& event )
{
	m_event = event;
	const TaskTreeItem& taskTreeItem =
        MODEL.charmDataModel()->taskTreeItem( m_event.taskId() );
	m_ui->labelTaskName->setText( tasknameWithParents( taskTreeItem.task() ) );
	m_ui->labelStart->setText( m_event.startDateTime().toString( Qt::SystemLocaleShortDate) );
	m_ui->labelEnd->setText( m_event.endDateTime().toString( Qt::SystemLocaleShortDate) );
	const int durationHours = m_event.duration() / 3600;
	const int durationMinutes = ( m_event.duration() % 3600 ) / 60;
	m_ui->labelDuration->setText( tr( "%1:%2")
		.arg( durationHours )
		.arg( durationMinutes ) );
	const int weeknumber = m_event.startDateTime().date().weekNumber();
	m_ui->labelWeeknumber->setText( tr("%1").arg( weeknumber ) );
	m_ui->labelComment->setText( m_event.comment() );
}

#include "EventDisplay.moc"
