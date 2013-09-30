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
#include "Commands/CommandMakeEvent.h"

#include "ui_EventEditor.h"

#include <QCalendarWidget>
#include <QSettings>

EventEditor::EventEditor( const Event& event, QWidget* parent )
    : QDialog( parent )
    , m_event( event )
    , m_updating( false )
    , m_endDateChanged( true )
{
    m_ui = new Ui::EventEditor();
    m_ui->setupUi( this );
    m_ui->dateEditEnd->calendarWidget()->setFirstDayOfWeek( Qt::Monday );
    m_ui->dateEditEnd->calendarWidget()->setVerticalHeaderFormat( QCalendarWidget::ISOWeekNumbers );
    m_ui->dateEditStart->calendarWidget()->setFirstDayOfWeek( Qt::Monday );
    m_ui->dateEditStart->calendarWidget()->setVerticalHeaderFormat( QCalendarWidget::ISOWeekNumbers );

    // Ctrl+Return for OK
    m_ui->buttonBox->button(QDialogButtonBox::Ok)->setShortcut(Qt::CTRL + Qt::Key_Return);

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
    connect( m_ui->startToNowButton, SIGNAL( clicked() ),
             SLOT( startToNowButtonClicked() ) );
    connect( m_ui->endToNowButton, SIGNAL( clicked() ),
             SLOT( endToNowButtonClicked() ) );
    // what a fricking hack - but QDateTimeEdit does not seem to have
    // a simple function to toggle 12h and 24h mode:
    // yeah, I know, this will survive changes in the user prefs, but
    // only for this instance of the edit dialog
    QString originalDateTimeFormat = m_ui->timeEditStart->displayFormat();

    QString format = originalDateTimeFormat
                     .replace( "ap", "" )
                     .replace( "AP", "" )
                     .simplified();
    m_ui->timeEditStart->setDisplayFormat( format );
    m_ui->timeEditEnd->setDisplayFormat( format );

    // initialize to some sensible values, unless we got something valid passed in
    if ( !m_event.isValid() ) {
        QSettings settings;
        QDateTime start = settings.value( MetaKey_LastEventEditorDateTime, QDateTime::currentDateTime() ).toDateTime();
        m_event.setStartDateTime( start );
        m_event.setEndDateTime( start );
        m_endDateChanged = false;
    }
    updateValues( true );
}

EventEditor::~EventEditor()
{
    delete m_ui; m_ui = 0;
}

void EventEditor::accept()
{
    QSettings settings;
    settings.setValue( MetaKey_LastEventEditorDateTime, m_event.endDateTime() );
    QDialog::accept();
}

Event EventEditor::eventResult() const
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
    int delta = m_event.startDateTime().secsTo( m_event.endDateTime() );
    m_event.setStartDateTime( start );
    if ( !m_endDateChanged ) {
        m_event.setEndDateTime( start.addSecs( delta ));
    }
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
    if ( !m_updating ) {
        m_endDateChanged = true;
    }
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

struct SignalBlocker {
    explicit SignalBlocker( QObject* o )
        : m_object( o )
        , m_previous( o->signalsBlocked() ) {
        o->blockSignals( true );
    }

    ~SignalBlocker() {
        m_object->blockSignals( m_previous );
    }

    QObject* m_object;
    bool m_previous;
};

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

    bool active = MODEL.charmDataModel()->isEventActive( m_event.id() );
    m_ui->dateEditEnd->setEnabled( !active );
    m_ui->timeEditEnd->setEnabled( !active );
    m_ui->textEditComment->setEnabled( !active );
    m_ui->spinBoxHours->setEnabled( !active );
    m_ui->spinBoxMinutes->setEnabled( !active );
    m_ui->pushButtonSelectTask->setEnabled( !active );
    m_ui->startToNowButton->setEnabled( !active );
    m_ui->endToNowButton->setEnabled( !active );

    if ( !active ) {
        m_ui->dateEditEnd->setDate( m_event.endDateTime().date() );
        m_ui->timeEditEnd->setTime( m_event.endDateTime().time() );
    } else {
        m_ui->dateEditEnd->setDate( QDate::currentDate() );
        m_ui->timeEditEnd->setTime( QTime::currentTime() );
    }
    if( all ) {
        m_ui->textEditComment->setText( m_event.comment() );
    }
    int durationHours = qMax( m_event.duration() / 3600, 0);
    int durationMinutes = qMax( ( m_event.duration() % 3600 ) / 60, 0 );

    { // block signals to prevent updates of the start/end edits
        const SignalBlocker blocker1( m_ui->spinBoxHours );
        const SignalBlocker blocker2( m_ui->spinBoxMinutes );
        m_ui->spinBoxHours->setValue( durationHours );
        m_ui->spinBoxMinutes->setValue( durationMinutes );
    }

    QString name = MODEL.charmDataModel()->fullTaskName( taskTreeItem.task() );
    m_ui->labelTaskName->setText( name );

    QString format = m_ui->dateEditStart->displayFormat()
                     .replace( "ap",  "" )
                     .replace( "AP",  "" )
                     .simplified();
    m_ui->dateEditStart->setDisplayFormat( format );
    m_ui->dateEditEnd->setDisplayFormat( format );

    m_updating = false;
}


void EventEditor::startToNowButtonClicked()
{
    m_event.setStartDateTime( QDateTime::currentDateTime() );
    updateValues();
}

void EventEditor::endToNowButtonClicked()
{
    m_event.setEndDateTime( QDateTime::currentDateTime() );
    updateValues();
}

#include "EventEditor.moc"

