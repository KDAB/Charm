#include <QtDebug>

#include <QMap>
#include <QMenu>
#include <QDateTime>
#include <QToolButton>

#include "Core/Task.h"
#include "Core/Event.h"
#include "Data.h"
#include "ViewHelpers.h"
#include "Reports/CharmReport.h"
#include "Charm/SelectTaskDialog.h"

#include "TimeTrackingTaskSelector.h"

#define CUSTOM_TASK_PROPERTY_NAME "CUSTOM_TASK_PROPERTY"

TimeTrackingTaskSelector::TimeTrackingTaskSelector(QWidget *parent)
    : QWidget(parent)
    , m_stopGoButton( new QToolButton( this ) )
    , m_taskSelectorButton( new QToolButton( this ) )
    , m_menu( new QMenu( m_taskSelectorButton ) )
    , m_selectedTask( 0 )
    , m_manuallySelectedTask( 0 )
    , m_taskManuallySelected( false )
{
    connect( m_menu, SIGNAL( triggered( QAction* ) ),
             SLOT( slotActionSelected( QAction* ) ) );
    m_menu->setSeparatorsCollapsible( true );
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

/** A helper function that takes an entry from the fromList if it is not empty, checks if it is
 * already contained in the list of visited tasks, and if not, prepends it into the targetList,
 * and creates a menu action.
 * All parameters may be modified.
 */
void insertHelper( QMenu* menu, TaskIdList& targetList, QMap<TaskId, QAction*>& visitedTasks, TaskIdList& fromList )
{
    if( ! fromList.isEmpty() ) {
        TaskId id = fromList.takeFirst();
        if( ! visitedTasks.contains( id ) ) {
            const Task& task = DATAMODEL->getTask( id );
            QAction* action = new QAction( tasknameWithParents( task ), menu );
            action->setProperty( CUSTOM_TASK_PROPERTY_NAME, QVariant::fromValue( id ) );
            menu->addAction( action );
            visitedTasks.insert( id, action );
            targetList.append( id );
        }
    }
}

void TimeTrackingTaskSelector::populate( const QVector<WeeklySummary>& summaries )
{
    m_menu->clear();
    QMap<TaskId, QAction*> visitedTasks;
    Q_FOREACH( const WeeklySummary& s, summaries ) {
        QAction* action = new QAction( s.taskname, m_menu );
        visitedTasks.insert( s.task, action );
        action->setProperty( CUSTOM_TASK_PROPERTY_NAME, QVariant::fromValue( s.task ) );
        Q_ASSERT( action->property( CUSTOM_TASK_PROPERTY_NAME ).value<TaskId>() == s.task );
        m_menu->addAction( action );
    }
    // insert the manually selected task, if one is set:
    m_menu->addSeparator();
    if( m_manuallySelectedTask > 0 && ! visitedTasks.contains( m_manuallySelectedTask )) {
        const Task& task = DATAMODEL->getTask( m_manuallySelectedTask );
        QAction* action = new QAction( tasknameWithParents( task ), m_menu );
        visitedTasks.insert( m_manuallySelectedTask, action );
        action->setProperty( CUSTOM_TASK_PROPERTY_NAME, QVariant::fromValue( m_manuallySelectedTask ) );
        m_menu->addAction( action );
    }
    // ... add action to select a task:
    QAction* selectTaskAction = new QAction( tr( "Select other task..." ), m_menu );
    connect( selectTaskAction, SIGNAL( triggered() ), SLOT( slotManuallySelectTask() ) );
    m_menu->addAction( selectTaskAction );
    // build a list of "interesting" tasks
    TaskIdList mru = DATAMODEL->mostRecentlyUsedTasks();
    TaskIdList mfu = DATAMODEL->mostFrequentlyUsedTasks();
    // ... merge the two lists into one interesting one:
    // add to menu
    m_menu->addSeparator();
    TaskIdList merged;
    while( merged.count() < 15 ) { // arbitrary hardcoded number warning
        insertHelper( m_menu, merged, visitedTasks, mru );
        insertHelper( m_menu, merged, visitedTasks, mru );
        insertHelper( m_menu, merged, visitedTasks, mfu );
        if( mru.isEmpty() && mfu.isEmpty() ) break;
    }
    // finally, select the task that the user has just selected
    if( m_taskManuallySelected ) {
        QAction* action = visitedTasks.value( m_manuallySelectedTask );
        Q_ASSERT_X( action != 0, Q_FUNC_INFO, "the manually selected task should always be in the menu" );
        // this sets the correct text on the button
        slotActionSelected( action );
        m_taskManuallySelected = false;
    }
    // enable the selector button if the menu is not empty
    m_taskSelectorButton->setDisabled( m_menu->actions().isEmpty() );
}

void TimeTrackingTaskSelector::handleActiveEvents()
{
    const int activeEventCount = DATAMODEL->activeEventCount();
    if ( activeEventCount > 1 ) {
        m_stopGoButton->setIcon( Data::goIcon() );
        m_stopGoButton->setText( tr( "Start" ) );
        m_taskSelectorButton->setEnabled( false );
        m_stopGoButton->setEnabled( false );
        m_stopGoButton->setChecked( true );
    } else if ( activeEventCount == 1 ) {
        m_stopGoButton->setIcon( Data::stopIcon() );
        m_stopGoButton->setText( tr( "Stop" ) );
        m_stopGoButton->setEnabled( true );
        m_taskSelectorButton->setEnabled( false );
        m_stopGoButton->setChecked( true );
    } else {
        m_stopGoButton->setIcon( Data::goIcon() );
        m_stopGoButton->setText( tr( "Start" ) );
        m_taskSelectorButton->setDisabled( m_menu->actions().isEmpty() );
        if( m_selectedTask != 0 ) {
            const Task& task = DATAMODEL->getTask( m_selectedTask );
            m_stopGoButton->setEnabled( task.isCurrentlyValid() );
        } else {
            m_stopGoButton->setEnabled( false );
        }
        m_stopGoButton->setChecked( false );
    }
}

void TimeTrackingTaskSelector::slotActionSelected( QAction* action )
{
    TaskId taskId = action->property( CUSTOM_TASK_PROPERTY_NAME ).value<TaskId>();
    if( taskId > 0 ) {
        taskSelected( action->text(), taskId );
        handleActiveEvents();
    }
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

void TimeTrackingTaskSelector::slotManuallySelectTask()
{
    SelectTaskDialog dialog( this );
    if( dialog.exec() ) {
        m_manuallySelectedTask = dialog.selectedTask();
        m_taskManuallySelected = true;
        emit updateSummariesPlease();
    }
}

#include "TimeTrackingTaskSelector.moc"
