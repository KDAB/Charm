/*
 * EventEditor.cpp
 *
 *  Created on: Jun 27, 2008
 *      Author: mirko
 */

#include "Core/CharmConstants.h"
#include "Core/TaskTreeItem.h"
#include "Core/CharmDataModel.h"

#include "ViewHelpers.h"
#include "EventEditor.h"
#include "SelectTaskDialog.h"
#include "Reports/CharmReport.h"

#include "ui_EventEditor.h"

EventEditor::EventEditor( const Event& event, QWidget* parent )
    : QDialog( parent )
    , m_event( event )
    , m_updating( false )
{
    m_ui = new Ui::EventEditor();
    m_ui->setupUi( this );
    // connect stuff:
    connect( m_ui->spinBoxHours, SIGNAL( valueChanged( int ) ),
             SLOT( durationHoursEdited( int ) ) );
    connect( m_ui->spinBoxMinutes, SIGNAL( valueChanged( int ) ),
             SLOT( durationMinutesEdited( int ) ) );
    connect( m_ui->dateEditStart, SIGNAL( dateChanged( QDate ) ),
             SLOT( startDateChanged( QDate ) ) );
    connect( m_ui->timeEditStart, SIGNAL( timeChanged( QTime ) ),
             SLOT( startTimeChanged( QTime ) ) );
    connect( m_ui->dateEditEnd, SIGNAL( dateChanged( QDate ) ),
             SLOT( endDateChanged( QDate ) ) );
    connect( m_ui->timeEditEnd, SIGNAL( timeChanged( QTime ) ),
             SLOT( endTimeChanged( QTime ) ) );
    connect( m_ui->pushButtonSelectTask, SIGNAL( clicked() ),
             SLOT( selectTaskClicked() ) );
    connect( m_ui->textEditComment, SIGNAL( textChanged() ),
             SLOT( commentChanged() ) );

    // what a fricking hack - but QDateTimeEdit does not seem to have
    // a simple function to toggle 12h and 24h mode:
    // yeah, I know, this will survive changes in the user prefs, but
    // only for this instance of the edit dialog
    QTimeEdit edit( this ); // this bugger is gone after the constructor
    QString originalDateTimeFormat = edit.displayFormat();

    if ( CONFIGURATION.always24hEditing ) {
        QString format = m_ui->timeEditStart->displayFormat()
                         .replace( "ap",  "" )
                         .replace( "AP",  "" )
                         .simplified();
        m_ui->timeEditStart->setDisplayFormat( format );
        m_ui->timeEditEnd->setDisplayFormat( format );
    } else {
        m_ui->timeEditStart->setDisplayFormat( originalDateTimeFormat );
        m_ui->timeEditEnd->setDisplayFormat( originalDateTimeFormat );
    }


    updateValues( true );
}

EventEditor::~EventEditor()
{
	delete m_ui; m_ui = 0;
}

Event EventEditor::event() const
{
	return m_event;
}

void EventEditor::durationHoursEdited( int value )
{
	updateEndTime();
	updateValues();
}

void EventEditor::durationMinutesEdited( int value )
{
	updateEndTime();
	updateValues();
}

void EventEditor::updateEndTime()
{
	int duration = 3600 * m_ui->spinBoxHours->value() + 60 * m_ui->spinBoxMinutes->value();
	QDateTime endTime = m_event.startDateTime().addSecs( duration );
	m_event.setEndDateTime( endTime );
}

void EventEditor::startDateChanged( const QDate& date )
{
	QDateTime start = m_event.startDateTime();
	start.setDate( date );
	m_event.setStartDateTime( start );
	updateValues();
}

void EventEditor::startTimeChanged( const QTime& time )
{
	QDateTime start = m_event.startDateTime();
	start.setTime( time );
	m_event.setStartDateTime( start );
	updateValues();
}

void EventEditor::endDateChanged( const QDate& date )
{
	QDateTime end = m_event.endDateTime();
	end.setDate( date );
	m_event.setEndDateTime( end );
	updateValues();
}

void EventEditor::endTimeChanged( const QTime& time )
{
	QDateTime end = m_event.endDateTime();
	end.setTime( time );
	m_event.setEndDateTime( end );
	updateValues();
}

void EventEditor::selectTaskClicked()
{
    SelectTaskDialog dialog( this );

    if ( dialog.exec() ) {
    	m_event.setTaskId( dialog.selectedTask() );
    	updateValues();
    }
}

void EventEditor::commentChanged()
{
	m_event.setComment( m_ui->textEditComment->toPlainText() );
	updateValues();
}

void EventEditor::updateValues( bool all )
{
	if( m_updating ) return;

	m_updating = true;

	m_ui->buttonBox->button( QDialogButtonBox::Ok )
		->setEnabled( m_event.endDateTime() >= m_event.startDateTime() );
	const TaskTreeItem& taskTreeItem =
        MODEL.charmDataModel()->taskTreeItem( m_event.taskId() );

    m_ui->dateEditStart->setDate( m_event.startDateTime().date() );
    m_ui->timeEditStart->setTime( m_event.startDateTime().time() );
    m_ui->dateEditEnd->setDate( m_event.endDateTime().date() );
    m_ui->timeEditEnd->setTime( m_event.endDateTime().time() );
    if( all ) {
    	m_ui->textEditComment->setText( m_event.comment() );
    }
    int durationHours = qMax( m_event.duration() / 3600, 0);
    int durationMinutes = qMax( ( m_event.duration() % 3600 ) / 60, 0 );
    m_ui->spinBoxHours->setValue( durationHours );
    m_ui->spinBoxMinutes->setValue( durationMinutes );
    QString name = tasknameWithParents( taskTreeItem.task() );
    m_ui->labelTaskName->setText( name );

    if ( CONFIGURATION.always24hEditing ) {
        QString format = m_ui->dateEditStart->displayFormat()
                         .replace( "ap",  "" )
                         .replace( "AP",  "" )
                         .simplified();
        m_ui->dateEditStart->setDisplayFormat( format );
        m_ui->dateEditEnd->setDisplayFormat( format );
    }

    m_updating = false;
}

#include "EventEditor.moc"

