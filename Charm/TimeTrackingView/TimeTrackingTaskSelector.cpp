#include <QMenu>
#include <QToolButton>

#include "Core/Task.h"
#include "Data.h"

#include "TimeTrackingTaskSelector.h"

#define CUSTOM_TASK_PROPERTY_NAME "CUSTOM_TASK_PROPERTY"

TimeTrackingTaskSelector::TimeTrackingTaskSelector(QWidget *parent)
    : QWidget(parent)
    , m_stopGoButton( new QToolButton( this ) )
    , m_menu( new QMenu() )
    , m_taskSelectorButton( new QToolButton( this ) )
    , m_selectedTask( 0 )
{
    connect( m_menu, SIGNAL( triggered( QAction* ) ),
             SLOT( slotActionSelected( QAction* ) ) );

    m_stopGoButton->setCheckable( true );
    connect( m_stopGoButton, SIGNAL( clicked( bool ) ),
             SLOT( slotGoStopToggled( bool ) ) );
    m_taskSelectorButton->setEnabled( false );
    m_taskSelectorButton->setPopupMode( QToolButton::InstantPopup );
    m_taskSelectorButton->setMenu( m_menu );
    m_taskSelectorButton->setText( tr( "Select Task" ) );
}

QSize TimeTrackingTaskSelector::sizeHint() const
{
    const QSize stopGoButtonSizeHint = m_stopGoButton->sizeHint();
    return QSize( 200, stopGoButtonSizeHint.height() ); // width is ignored anyway
}

void TimeTrackingTaskSelector::resizeEvent( QResizeEvent* )
{
    m_stopGoButton->resize( m_stopGoButton->sizeHint() );
    m_stopGoButton->move( 0, 0 );
    const QSize space( width() - m_stopGoButton->width(), height() );
    m_taskSelectorButton->resize( space );
    m_taskSelectorButton->move( m_stopGoButton->width(), 0 );
}

void TimeTrackingTaskSelector::populate( const QVector<WeeklySummary>& summaries )
{
    m_menu->clear();
    m_taskSelectorButton->setDisabled( summaries.isEmpty() );
    Q_FOREACH( const WeeklySummary& s, summaries ) {
        QAction* action = new QAction( s.taskname, m_menu );
        action->setProperty( CUSTOM_TASK_PROPERTY_NAME, QVariant::fromValue( s.task ) );
        Q_ASSERT( action->property( CUSTOM_TASK_PROPERTY_NAME ).value<TaskId>() == s.task );
        m_menu->addAction( action );
    }
    // FIXME add most-recently-used list
}

void TimeTrackingTaskSelector::handleActiveEvents( int activeEventCount, const QVector<WeeklySummary>& summaries )
{
    if ( activeEventCount > 1 ) {
        m_stopGoButton->setIcon( Data::recorderGoIcon() );
        m_stopGoButton->setText( tr( "Start" ) );
        m_taskSelectorButton->setEnabled( false );
        m_stopGoButton->setEnabled( false );
        m_stopGoButton->setChecked( true );
    } else if ( activeEventCount == 1 ) {
        m_stopGoButton->setIcon( Data::recorderStopIcon() );
        m_stopGoButton->setText( tr( "Stop" ) );
        m_stopGoButton->setEnabled( true );
        m_taskSelectorButton->setEnabled( false );
        m_stopGoButton->setChecked( true );
    } else {
        m_stopGoButton->setIcon( Data::recorderGoIcon() );
        m_stopGoButton->setText( tr( "Start" ) );
        m_taskSelectorButton->setEnabled( !summaries.isEmpty() );
        m_stopGoButton->setEnabled( m_selectedTask != 0 );
        m_stopGoButton->setChecked( false );
    }
}

void TimeTrackingTaskSelector::slotActionSelected( QAction* action )
{
    TaskId taskId = action->property( CUSTOM_TASK_PROPERTY_NAME ).value<TaskId>();
    taskSelected( action->text(), taskId );
}

void TimeTrackingTaskSelector::taskSelected( const QString& taskname, TaskId id )
{
    m_selectedTask = id;
    m_stopGoButton->setEnabled( true );
    m_taskSelectorButton->setText( taskname );
}

void TimeTrackingTaskSelector::slotGoStopToggled( bool on )
{
    Q_ASSERT( m_selectedTask != 0 );
    if( on ) {
        emit startEvent( m_selectedTask );
    } else {
        emit stopEvent( m_selectedTask );
    }
}

void TimeTrackingTaskSelector::taskSelected( const WeeklySummary& summary )
{
    taskSelected( summary.taskname, summary.task );
}

#include "TimeTrackingTaskSelector.moc"
