#include <QtDebug>

#include <QAction>
#include <QMap>
#include <QMenu>
#include <QDateTime>
#include <QToolButton>
#include <QVBoxLayout>
#include <QPointer>
#include <QLineEdit>
#include <QTextEdit>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QToolBar>

#include "Core/Task.h"
#include "Core/Event.h"
#include "Data.h"
#include "ViewHelpers.h"
#include "Charm/SelectTaskDialog.h"

#include "TimeTrackingTaskSelector.h"

#define CUSTOM_TASK_PROPERTY_NAME "CUSTOM_TASK_PROPERTY"

CommentEditorPopup::CommentEditorPopup( QWidget* parent )
    : QDialog( parent )
    , m_edit( new QTextEdit )
{
    setWindowTitle( tr("Comment Event") );
    const EventIdList events = DATAMODEL->activeEvents();
    Q_ASSERT( events.size() == 1 );
    m_id = events.first();
    QVBoxLayout* layout = new QVBoxLayout( this );
    layout->setMargin( 0 );
    m_edit = new QTextEdit;
    m_edit->setTabChangesFocus( true );
    m_edit->setPlainText( DATAMODEL->eventForId( m_id ).comment() );
    layout->addWidget( m_edit );
    QDialogButtonBox* box = new QDialogButtonBox;
    box->setStandardButtons( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
    connect( box, SIGNAL(accepted()), this, SLOT(accept()) );
    connect( box, SIGNAL(rejected()), this, SLOT(reject()) );
    layout->addWidget( box );
    m_edit->setFocus( Qt::TabFocusReason );
}

void CommentEditorPopup::accept() {
    const QString t = m_edit->toPlainText();
    Event event = DATAMODEL->eventForId( m_id );
    if ( event.isValid() ) {
        event.setComment( t );
        DATAMODEL->modifyEvent( event );
    } else { // event already gone? should never happen, but you never know
        QPointer<QObject> that( this );
        QMessageBox::critical( this, tr("Error"), tr("Could not save the comment, the edited event was deleted in the meantime."), QMessageBox::Ok );
        if ( !that ) // in case the popup was deleted while the msg box was open
            return;
    }
    QDialog::accept();
}

TimeTrackingTaskSelector::TimeTrackingTaskSelector(QToolBar* toolBar, QWidget *parent)
    : QWidget(parent)
    , m_stopGoButton( new QToolButton( this ) )
    , m_stopGoAction( new QAction( this ) )
    , m_editCommentButton( new QToolButton( this ) )
    , m_editCommentAction( new QAction( this ) )
    , m_taskSelectorButton( new QToolButton( this ) )
    , m_menu( new QMenu( tr( "Start Task" ), m_taskSelectorButton ) )
    , m_selectedTask( 0 )
    , m_manuallySelectedTask( 0 )
    , m_taskManuallySelected( false )
    , m_startSelectedTask( true )
{
    toolBar->hide();
    connect( m_menu, SIGNAL( triggered( QAction* ) ),
             SLOT( slotActionSelected( QAction* ) ) );

    m_stopGoAction->setIcon( Data::goIcon() );
    m_stopGoAction->setShortcut( QKeySequence( Qt::Key_Space ) );
    m_stopGoAction->setCheckable( true );
    connect( m_stopGoAction, SIGNAL(triggered(bool)), SLOT(slotGoStopToggled(bool)) );
    m_stopGoButton->setDefaultAction( m_stopGoAction );

    m_editCommentAction->setIcon( Data::editEventIcon() );
    m_editCommentAction->setShortcut( QKeySequence( Qt::Key_Return ) );
    m_editCommentAction->setToolTip( tr( "Comment on the running task" ) );
    connect( m_editCommentAction, SIGNAL( triggered(bool) ),
             SLOT( slotEditCommentClicked() ) );
    m_editCommentButton->setDefaultAction( m_editCommentAction );

    m_taskSelectorButton->setEnabled( false );
    m_taskSelectorButton->setPopupMode( QToolButton::InstantPopup );
    m_taskSelectorButton->setMenu( m_menu );
    m_taskSelectorButton->setText( tr( "Select Task" ) );
    m_taskSelectorButton->installEventFilter( this );
    m_menu->installEventFilter( this );
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
            QAction* action = new QAction( DATAMODEL->fullTaskName( task ), menu );
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
    bool addedAction = false;
    Q_FOREACH( const WeeklySummary& s, summaries ) {
        QAction* action = new QAction( s.taskname, m_menu );
        visitedTasks.insert( s.task, action );
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
    if( m_manuallySelectedTask > 0 && ! visitedTasks.contains( m_manuallySelectedTask )) {
        const Task& task = DATAMODEL->getTask( m_manuallySelectedTask );
        QAction* action = new QAction( DATAMODEL->fullTaskName( task ), m_menu );
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
    if ( addedAction ) {
        m_menu->addSeparator();
        addedAction = false;
    }
    TaskIdList merged;
    while( merged.count() < 15 ) { // arbitrary hardcoded number warning
        insertHelper( m_menu, merged, visitedTasks, mru );
        insertHelper( m_menu, merged, visitedTasks, mru );
        insertHelper( m_menu, merged, visitedTasks, mfu );
        addedAction = true;
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

void TimeTrackingTaskSelector::slotEditCommentClicked() {
    QPointer<CommentEditorPopup> popup( new CommentEditorPopup( this ) );
    popup->exec();
    delete popup;
}

void TimeTrackingTaskSelector::handleActiveEvents()
{
    Task task;
    if( m_selectedTask != 0 ) {
        task = DATAMODEL->getTask( m_selectedTask );
    }

    const int activeEventCount = DATAMODEL->activeEventCount();
    if ( activeEventCount > 1 ) {
        m_stopGoAction->setIcon( Data::goIcon() );
        m_stopGoAction->setText( tr( "Start" ) );
        m_taskSelectorButton->setEnabled( false );
        m_stopGoAction->setEnabled( false );
        m_stopGoAction->setChecked( true );
        m_editCommentAction->setEnabled( false );
    } else if ( activeEventCount == 1 ) {
        m_stopGoAction->setIcon( Data::stopIcon() );
        m_stopGoAction->setText( tr( "Stop" ) );
        m_stopGoAction->setEnabled( true );
        m_taskSelectorButton->setEnabled( false );
        m_menu->setEnabled( true );
        m_stopGoAction->setChecked( true );
        m_editCommentAction->setEnabled( true );
    } else {
        m_stopGoAction->setIcon( Data::goIcon() );
        m_stopGoAction->setText( tr( "Start" ) );
        m_taskSelectorButton->setDisabled( m_menu->actions().isEmpty() );
        m_menu->setEnabled( true );
        m_stopGoAction->setEnabled( task.isCurrentlyValid() );
        m_stopGoAction->setChecked( false );
        m_editCommentAction->setEnabled( false );
    }

    static Task previousTask;
    if ( task.isCurrentlyValid() && task != previousTask
            && !DATAMODEL->isTaskActive( task.id() )
            && m_startSelectedTask ) {
        if ( CONFIGURATION.oneEventAtATime )
            emit stopEvents();
        previousTask = task;
        emit startEvent( task.id() );
        m_stopGoAction->setEnabled( true );
    } else {
        previousTask = task;
        m_startSelectedTask = true;
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
    m_stopGoAction->setEnabled( true );
    m_taskSelectorButton->setText( taskname );
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

bool TimeTrackingTaskSelector::eventFilter( QObject* object, QEvent* event )
{
    // Don't start tasks we select in the time tracking view.
    if ( object == m_taskSelectorButton
            && event->type() == QEvent::MouseButtonPress )
        m_startSelectedTask = false;

    return QWidget::eventFilter(object, event);
}


#include "TimeTrackingTaskSelector.moc"
