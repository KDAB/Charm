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
#include <QPushButton>

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
    setWindowModality( Qt::WindowModal );
    setWindowTitle( tr("Comment Event") );
    const EventIdList events = DATAMODEL->activeEvents();
    Q_ASSERT( events.size() == 1 );
    m_id = events.first();
    auto layout = new QVBoxLayout( this );
    layout->setMargin( 0 );
    m_edit->setTabChangesFocus( true );
    m_edit->setPlainText( DATAMODEL->eventForId( m_id ).comment() );
    layout->addWidget( m_edit );
    auto box = new QDialogButtonBox;
    box->setStandardButtons( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
    box->button( QDialogButtonBox::Ok )->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_Return ) );
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
    , m_startOtherTaskAction( new QAction( tr( "Start Other Task..." ), this ) )
    , m_menu( new QMenu( tr( "Start Task" ), this ) )
    , m_selectedTask( 0 )
    , m_manuallySelectedTask( 0 )
    , m_taskManuallySelected( false )
{
    toolBar->hide();
    connect( m_menu, SIGNAL( triggered( QAction* ) ),
             SLOT( slotActionSelected( QAction* ) ) );

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
    connect( m_editCommentAction, SIGNAL( triggered(bool) ),
             SLOT( slotEditCommentClicked() ) );
    m_editCommentButton->setDefaultAction( m_editCommentAction );

    m_taskSelectorButton->setPopupMode( QToolButton::InstantPopup );
    m_taskSelectorButton->setMenu( m_menu );
    m_taskSelectorButton->setText( tr( "Select Task" ) );

    m_startOtherTaskAction->setShortcut( Qt::Key_T );
    connect( m_startOtherTaskAction, SIGNAL( triggered() ),
             SLOT( slotManuallySelectTask() ) );
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

void TimeTrackingTaskSelector::populate( const QVector<WeeklySummary>& summaries )
{
    // Don't repopulate while the menu is displayed; very ugly and it can wait.
    if (m_menu->isActiveWindow())
        return;

    m_menu->clear();
    QMap<TaskId, QAction*> addedTasks;
    bool addedAction = false;
    Q_FOREACH( const WeeklySummary& s, summaries ) {
        auto action = new QAction( DATAMODEL->taskIdAndSmartNameString( s.task ), m_menu );
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
        if( !addedTasks.contains( id ) )
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
        Q_ASSERT_X( action != nullptr, Q_FUNC_INFO, "the manually selected task should always be in the menu" );
        // this sets the correct text on the button
        slotActionSelected( action );
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


#include "TimeTrackingTaskSelector.moc"
