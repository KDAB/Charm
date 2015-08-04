/*
  TimeTrackingTaskSelector.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Frank Osterfeld <frank.osterfeld@kdab.com>
  Author: Montel Laurent <laurent.montel@kdab.com>

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

#include "TimeTrackingTaskSelector.h"
#include "CommentEditorPopup.h"
#include "Data.h"
#include "SelectTaskDialog.h"
#include "ViewHelpers.h"

#include "Core/Event.h"
#include "Core/Task.h"

#include <QAction>
#include <QDialogButtonBox>
#include <QMap>
#include <QMenu>
#include <QMessageBox>
#include <QPointer>
#include <QPushButton>
#include <QTextEdit>
#include <QToolButton>
#include <QVBoxLayout>

#define CUSTOM_TASK_PROPERTY_NAME "CUSTOM_TASK_PROPERTY"

TimeTrackingTaskSelector::TimeTrackingTaskSelector(QWidget *parent)
    : QWidget(parent)
    , m_stopGoButton( new QToolButton( this ) )
    , m_stopGoAction( new QAction( this ) )
    , m_editCommentButton( new QToolButton( this ) )
    , m_editCommentAction( new QAction( this ) )
    , m_taskSelectorButton( new QToolButton( this ) )
    , m_startOtherTaskAction( new QAction( tr( "Start Other Task..." ), this ) )
    , m_menu( new QMenu( tr( "Start Task" ), this ) )
    , m_selectedTask( 0 )
    , m_manuallySelectedTask( 0 )
    , m_taskManuallySelected( false )
{
    connect( m_menu, SIGNAL(triggered(QAction*)),
             SLOT(slotActionSelected(QAction*)) );

    m_stopGoAction->setText( tr("Start Task") );
    m_stopGoAction->setIcon( Data::goIcon() );
    m_stopGoAction->setShortcut( QKeySequence( Qt::Key_Space ) );
    m_stopGoAction->setCheckable( true );
    connect( m_stopGoAction, SIGNAL(triggered(bool)), SLOT(slotGoStopToggled(bool)) );
    m_stopGoButton->setDefaultAction( m_stopGoAction );

    m_editCommentAction->setText( tr("Edit Comment") );
    m_editCommentAction->setIcon( Data::editEventIcon() );
    m_editCommentAction->setShortcut( Qt::Key_E );
    m_editCommentAction->setToolTip( m_editCommentAction->text() );
    connect( m_editCommentAction, SIGNAL(triggered(bool)),
             SLOT(slotEditCommentClicked()) );
    m_editCommentButton->setDefaultAction( m_editCommentAction );

    m_taskSelectorButton->setPopupMode( QToolButton::InstantPopup );
    m_taskSelectorButton->setMenu( m_menu );
    m_taskSelectorButton->setText( tr( "Select Task" ) );

    m_startOtherTaskAction->setShortcut( Qt::Key_T );
    connect( m_startOtherTaskAction, SIGNAL(triggered()),
             SLOT(slotManuallySelectTask()) );
}

void TimeTrackingTaskSelector::populateEditMenu( QMenu* menu )
{
    menu->addAction( m_stopGoAction );
    menu->addAction( m_editCommentAction );
    menu->addAction( m_startOtherTaskAction );
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
    m_editCommentButton->resize( m_editCommentButton->sizeHint() );
    m_editCommentButton->move( m_stopGoButton->width(), 0 );
    const QSize space( width() - m_stopGoButton->width() - m_editCommentButton->width(), height() );
    m_taskSelectorButton->resize( space );
    m_taskSelectorButton->move( m_stopGoButton->width() + m_editCommentButton->width(), 0 );
}

QMenu* TimeTrackingTaskSelector::menu() const
{
    return m_menu;
}

static QString escapeAmpersands( QString text )
{
    text.replace( QLatin1String("&"), QLatin1String("&&") );
    return text;
}

void TimeTrackingTaskSelector::populate( const QVector<WeeklySummary>& summaries )
{
    // Don't repopulate while the menu is displayed; very ugly and it can wait.
    if (m_menu->isActiveWindow())
        return;

    m_menu->clear();
    QMap<TaskId, QAction*> addedTasks;
    bool addedAction = false;
    Q_FOREACH( const WeeklySummary& s, summaries ) {
        auto action = new QAction( escapeAmpersands( DATAMODEL->taskIdAndSmartNameString( s.task ) ), m_menu );
        addedTasks.insert( s.task, action );
        action->setProperty( CUSTOM_TASK_PROPERTY_NAME, QVariant::fromValue( s.task ) );
        Q_ASSERT( action->property( CUSTOM_TASK_PROPERTY_NAME ).value<TaskId>() == s.task );
        m_menu->addAction( action );
        addedAction = true;
    }
    // insert the manually selected task, if one is set:
    if ( addedAction ) {
        m_menu->addSeparator();
        addedAction = false;
    }
    if( m_manuallySelectedTask > 0 && ! addedTasks.contains( m_manuallySelectedTask )) {
        const Task& task = DATAMODEL->getTask( m_manuallySelectedTask );
        auto action = new QAction( DATAMODEL->taskIdAndSmartNameString( task.id() ), m_menu );
        addedTasks.insert( m_manuallySelectedTask, action );
        action->setProperty( CUSTOM_TASK_PROPERTY_NAME, QVariant::fromValue( m_manuallySelectedTask ) );
        m_menu->addAction( action );
    }
    // ... add action to select a task:
    m_menu->addAction( m_startOtherTaskAction );

    TaskIdList interestingTasks;
    interestingTasks += DATAMODEL->mostRecentlyUsedTasks();
    interestingTasks += DATAMODEL->mostFrequentlyUsedTasks();

    TaskIdList interestingTasksToAdd;
    while( interestingTasksToAdd.count() < 10 ) { // arbitrary hardcoded number warning
        if( interestingTasks.isEmpty() )
            break;

        TaskId id = interestingTasks.takeFirst();
        if( !addedTasks.contains( id ) && DATAMODEL->getTask(id).isCurrentlyValid() )
            interestingTasksToAdd.append( id );
    }

    qSort( interestingTasksToAdd.begin(), interestingTasksToAdd.end() );
    foreach( TaskId id, interestingTasksToAdd ) {
        if( addedTasks.contains( id ) )
            continue;
        if( !addedAction ) {
            m_menu->addSeparator();
            addedAction = true;
        }
        auto action = new QAction( DATAMODEL->taskIdAndSmartNameString( id ), m_menu );
        action->setProperty( CUSTOM_TASK_PROPERTY_NAME, QVariant::fromValue( id ) );
        m_menu->addAction( action );
        addedTasks.insert( id, action );
    }

    // finally, select the task that the user has just selected
    if( m_taskManuallySelected ) {
        m_taskManuallySelected = false;
        auto action = addedTasks.value( m_manuallySelectedTask );
        Q_ASSERT_X( action != 0, Q_FUNC_INFO, "the manually selected task should always be in the menu" );
        // this sets the correct text on the button
        slotActionSelected( action );
    }
    // enable the selector button if the menu is not empty
    m_taskSelectorButton->setDisabled( m_menu->actions().isEmpty() );
}

void TimeTrackingTaskSelector::slotEditCommentClicked() {
    const EventIdList events = DATAMODEL->activeEvents();
    Q_ASSERT( events.size() == 1 );
    CommentEditorPopup popup;
    popup.loadEvent( events.first() );
    popup.exec();
}

void TimeTrackingTaskSelector::handleActiveEvents()
{
    const int activeEventCount = DATAMODEL->activeEventCount();
    if ( activeEventCount > 1 ) {
        m_stopGoAction->setIcon( Data::goIcon() );
        m_stopGoAction->setText( tr( "Start Task" ) );
        m_stopGoAction->setEnabled( false );
        m_stopGoAction->setChecked( true );
        m_editCommentAction->setEnabled( false );
    } else if ( activeEventCount == 1 ) {
        m_stopGoAction->setIcon( Data::stopIcon() );
        m_stopGoAction->setText( tr( "Stop Task" ) );
        m_stopGoAction->setEnabled( true );
        m_stopGoAction->setChecked( true );
        m_editCommentAction->setEnabled( true );
    } else {
        m_stopGoAction->setIcon( Data::goIcon() );
        m_stopGoAction->setText( tr( "Start Task" ) );
        if( m_selectedTask != 0 ) {
            const Task& task = DATAMODEL->getTask( m_selectedTask );
            m_stopGoAction->setEnabled( task.isCurrentlyValid() );
        } else {
            m_stopGoAction->setEnabled( false );
        }
        m_stopGoAction->setChecked( false );
        m_editCommentAction->setEnabled( false );
    }
}

void TimeTrackingTaskSelector::slotActionSelected( QAction* action )
{
    TaskId taskId = action->property( CUSTOM_TASK_PROPERTY_NAME ).value<TaskId>();
    const Task& task = DATAMODEL->getTask( taskId );
    if ( task.isValid() ) {
        bool expired = !task.isCurrentlyValid();
        bool trackable = task.trackable();
        bool notTrackableAndExpired = ( !trackable && expired );
        int id = task.id();
        const QString name = task.name();
        const QString expirationDate = QLocale::system().toString(task.validUntil(), QLocale::ShortFormat);

        if ( !trackable || expired ) {
            QString message = notTrackableAndExpired ? tr( "The task %1 (%2) is not trackable and expired since %3").arg( id ).arg( name ).arg( expirationDate ) :
                                                       expired ? tr( "The task %1 (%2) is expired since %3").arg( id ).arg( name ).arg( expirationDate ) :
                                                                 tr( "The task %1 (%2) is not trackable").arg( id ).arg( name );

            QMessageBox::information( this, tr( "Please choose another task" ), message );
            return;
        }
    }

    if( taskId > 0 ) {
        taskSelected( action->text(), taskId );
        handleActiveEvents();

        if ( !DATAMODEL->isTaskActive( taskId ) ) {
            if ( !DATAMODEL->activeEvents().isEmpty() )
                emit stopEvents();
            emit startEvent( taskId );
        }
    }
}

void TimeTrackingTaskSelector::taskSelected( const QString& taskname, TaskId id )
{
    m_selectedTask = id;
    m_stopGoAction->setEnabled( true );
    m_taskSelectorButton->setText( escapeAmpersands( taskname ) );
}

void TimeTrackingTaskSelector::slotGoStopToggled( bool on )
{
    if( on ) {
        Q_ASSERT( m_selectedTask );
        emit startEvent( m_selectedTask );
    } else {
        emit stopEvents();
    }
}

void TimeTrackingTaskSelector::taskSelected( const WeeklySummary& summary )
{
    taskSelected( summary.taskname, summary.task );
}

void TimeTrackingTaskSelector::slotManuallySelectTask()
{
    SelectTaskDialog dialog( this );
    if( !dialog.exec() )
        return;
    m_manuallySelectedTask = dialog.selectedTask();
    if ( m_selectedTask <= 0 )
        m_selectedTask = m_manuallySelectedTask;
    m_taskManuallySelected = true;
    handleActiveEvents();
    emit updateSummariesPlease();
}


#include "moc_TimeTrackingTaskSelector.cpp"
