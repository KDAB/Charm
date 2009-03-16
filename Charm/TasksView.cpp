#include <QMenu>
#include <QPoint>
#include <QAction>
#include <QtDebug>
#include <QToolBar>
#include <QSettings>
#include <QTreeView>
#include <QCloseEvent>
#include <QMessageBox>
#include <QHeaderView>
#include <QItemSelectionModel>

#include "Data.h"
#include "Core/Task.h"
#include "Core/CharmConstants.h"
#include "TasksView.h"
#include "TaskEditor.h"
#include <QInputDialog>
#include "ViewHelpers.h"
#include "Core/State.h"
#include "GUIState.h"
#include "ViewFilter.h"
#include "Application.h"
#include "TaskIdDialog.h"
#include "TasksViewDelegate.h"

#include "Core/CharmCommand.h"
#include "Commands/CommandRelayCommand.h"
#include "Commands/CommandAddTask.h"
#include "Commands/CommandModifyTask.h"
#include "Commands/CommandDeleteTask.h"

#include "ui_TasksView.h"

TasksView::TasksView( QWidget* parent )
    : QWidget( parent )
    // , ViewInterface()
    , m_ui( new Ui::TasksView )
    , m_delegate( new TasksViewDelegate( this ) )
    , m_actionStartEvent( this )
    , m_actionEndEvent( this )
    , m_actionNewTask( this )
    , m_actionNewSubTask( this )
    , m_actionEditTask( this )
    , m_actionDeleteTask( this )
{
    m_ui->setupUi( this );
    m_ui->treeView->setItemDelegate( m_delegate );
    connect( m_delegate, SIGNAL( editingStateChanged() ),
             SLOT( configureUi() ) );
    m_ui->buttonClearFilter->setEnabled( false );
    m_ui->buttonClearFilter->setText( tr( "Clear Filter" ) );
    m_ui->buttonClearFilter->setIcon( Data::clearFilterIcon() );
    // set up actions
    // (no menu icons, please) m_actionAboutDialog.setIcon( Data::charmIcon() );
    m_actionStartEvent.setIcon( Data::goIcon() );
    m_actionStartEvent.setText( tr( "Start Task" ) );
    m_actionStartEvent.setShortcut( Qt::CTRL + Qt::Key_Return );
    m_ui->goButton->setDefaultAction( &m_actionStartEvent );
    connect( &m_actionStartEvent, SIGNAL( triggered( bool ) ),
             SLOT( actionStartEvent() ) );

    m_actionEndEvent.setIcon( Data::stopIcon() );
    m_actionEndEvent.setText( tr( "Stop Task" ) );
    m_ui->stopButton->setDefaultAction( &m_actionEndEvent );
    connect( &m_actionEndEvent, SIGNAL( triggered( bool ) ),
             SLOT( actionEndEvent() ) );

    m_actionNewTask.setText( tr( "New &Task" ) );
    m_actionNewTask.setShortcut( QKeySequence::New );
    m_actionNewTask.setIcon( Data::newTaskIcon() );
    connect( &m_actionNewTask, SIGNAL( triggered( bool ) ),
             SLOT( actionNewTask() ) );

    m_actionNewSubTask.setText( tr( "New &Subtask" ) );
    m_actionNewSubTask.setShortcut( Qt::META + Qt::Key_N );
    m_actionNewSubTask.setIcon( Data::newTaskIcon() );
    connect( &m_actionNewSubTask, SIGNAL( triggered( bool ) ),
             SLOT( actionNewSubTask() ) );

    m_actionEditTask.setText( tr( "Edit Task" ) );
    m_actionEditTask.setShortcut( Qt::CTRL + Qt::Key_E );
    m_actionEditTask.setIcon( Data::editTaskIcon() );
    connect( &m_actionEditTask, SIGNAL( triggered( bool ) ),
             SLOT( actionEditTask() ) );

    m_actionDeleteTask.setText( tr( "Delete Task" ) );
#ifdef Q_WS_MAC
    m_actionDeleteTask.setShortcut( Qt::Key_Backspace );
#else
    m_actionDeleteTask.setShortcut( QKeySequence::Delete );
#endif
    m_actionDeleteTask.setIcon( Data::deleteTaskIcon() );
    connect( &m_actionDeleteTask, SIGNAL( triggered( bool ) ),
             SLOT( actionDeleteTask() ) );

    // filter setup
    connect( m_ui->filterLineEdit, SIGNAL( textChanged( const QString& ) ),
             SLOT( slotFiltertextChanged( const QString& ) ) );
    connect( m_ui->tasksCombo, SIGNAL( currentIndexChanged( int ) ),
             SLOT( taskPrefilteringChanged( int ) ) );
    m_ui->treeView->setContextMenuPolicy( Qt::CustomContextMenu );
    connect( m_ui->treeView, SIGNAL( customContextMenuRequested( const QPoint& ) ),
             SLOT( slotContextMenuRequested( const QPoint& ) ) );

    connect( m_ui->treeView, SIGNAL( doubleClicked( const QModelIndex& ) ),
             SLOT( slotItemDoubleClicked( const QModelIndex& ) ) );
    m_ui->tasksCombo->setCurrentIndex( 0 );
}

TasksView::~TasksView()
{
    delete m_ui; m_ui = 0;
}

void TasksView::populateEditMenu( QMenu* editMenu )
{
    editMenu->addAction( &m_actionStartEvent );
    editMenu->addAction( &m_actionEndEvent );
    editMenu->addSeparator();
    editMenu->addAction( &m_actionNewTask );
    editMenu->addAction( &m_actionNewSubTask );
    editMenu->addAction( &m_actionEditTask );
    editMenu->addAction( &m_actionDeleteTask );
}

void TasksView::actionStartEvent()
{
    ViewFilter* filter = Application::instance().model().taskModel();
    Task task = selectedTask();
    // respect configuration:
    if ( CONFIGURATION.eventsInLeafsOnly && filter->taskHasChildren( task ) )
        return;

    // emit signal:
    if ( task.isValid() )
    {
        CharmDataModel* model = MODEL.charmDataModel();
        model->startEventRequested( task );
    }
}

void TasksView::actionEndEvent()
{
    Task task = selectedTask();
    // emit signal:
    if ( task.isValid() )
    {
        CharmDataModel* model = MODEL.charmDataModel();
        model->endEventRequested( task );
    }
}

void TasksView::actionNewTask()
{
    addTaskHelper( Task() );
}

void TasksView::actionNewSubTask()
{
    Task task = selectedTask();
    Q_ASSERT( task.isValid() );
    addTaskHelper( task );
}

void TasksView::actionEditTask()
{
    Task task = selectedTask();
    Q_ASSERT( task.isValid() );

    TaskEditor editor( this );
    editor.setTask( task );
    if( editor.exec() ) {
        const Task newTask = editor.getTask();
        newTask.dump();
        CommandModifyTask* cmd = new CommandModifyTask( newTask, this );
        emit emitCommand( cmd );
    }
    configureUi(); // FIXME needed?
}

void TasksView::actionDeleteTask()
{
    Task task = selectedTask();
    if ( QMessageBox::question( this, tr( "Delete Task?" ),
                                tr( "Do you really want to delete this task?\n"
                                    "Warning: All events for this task will be deleted as well!\n"
                                    "This operation cannot be undone." ),
                                QMessageBox::Ok | QMessageBox::Cancel,
                                QMessageBox::Ok )
         == QMessageBox::Ok ) {
        CommandDeleteTask* cmd = new CommandDeleteTask( task, this );
        emit emitCommand( cmd );
    }
}

void TasksView::addTaskHelper( const Task& parent )
{
    ViewFilter* filter = Application::instance().model().taskModel();
    Task task;
    int suggestedId = 1;
    if ( parent.isValid() ) {
        task.setParent( parent.id() );
        // subscribe if the parent is subscribed:
        task.setSubscribed( parent.subscribed()
                            || CONFIGURATION.taskPrefilteringMode == Configuration::TaskPrefilter_SubscribedOnly
                            || CONFIGURATION.taskPrefilteringMode == Configuration::TaskPrefilter_SubscribedAndCurrentOnly );
    }
    // yeah, daredevil!
    while ( filter->taskIdExists( suggestedId ) ) ++suggestedId;
    TaskIdDialog dialog( filter, this );
    dialog.setSuggestedId( suggestedId );
    if ( dialog.exec() ) {
        task.setId( dialog.selectedId() );
        task.setName( dialog.taskName() );
        CommandAddTask* cmd = new CommandAddTask( task, this );
        emit emitCommand( cmd );
    }
}

void TasksView::configureUi()
{
    const QItemSelectionModel* smodel = m_ui->treeView->selectionModel();
    const QModelIndex current = smodel ? smodel->currentIndex() : QModelIndex();
    const ViewFilter* filter = Application::instance().model().taskModel();
    const bool editing = m_delegate->isEditing();
    const bool selected = current.isValid();
    const Task task = selected ? filter->taskForIndex( current ) : Task();
    const bool active = filter->taskIsActive( task );
    const bool isCurrent = task.isCurrentlyValid();
    const bool hasChildren = filter->taskHasChildren( task );
    const bool cannotStart = hasChildren && CONFIGURATION.eventsInLeafsOnly;
    const bool cannotStop = m_delegate->isEditing();

    m_actionDeleteTask.setEnabled( selected && ! hasChildren );
    m_actionEditTask.setEnabled( selected );
    m_actionNewSubTask.setEnabled( selected );
    m_actionStartEvent.setEnabled( selected && ! cannotStart && ! active && isCurrent && ! editing );
    m_actionEndEvent.setEnabled( selected && active && ! cannotStop );
}

 void TasksView::closeEvent( QCloseEvent* )
 {
     saveGuiState();
 }

 void TasksView::showEvent( QShowEvent* )
 {
     restoreGuiState();
 }

 void TasksView::stateChanged( State previous )
 {
     switch( Application::instance().state() ) {
     case Connecting:
     {
         // set model on view:
         ViewFilter* filter = Application::instance().model().taskModel();
         m_ui->treeView->setModel( filter );
         const QItemSelectionModel* smodel =  m_ui->treeView->selectionModel();
         connect( smodel, SIGNAL( currentChanged( const QModelIndex&, const QModelIndex& ) ), SLOT( configureUi() ) );
         connect( smodel, SIGNAL( selectionChanged( const QItemSelection&, const QItemSelection& ) ), SLOT( configureUi() ) );
         connect( filter, SIGNAL( eventActivationNotice( EventId ) ),
                  SLOT( slotEventActivated( EventId ) ) );
         connect( filter, SIGNAL( eventDeactivationNotice( EventId ) ),
                  SLOT( slotEventDeactivated( EventId ) ) );
     }
     break;
     case Connected:
         restoreGuiState();
         break;
     case Disconnecting:
     case ShuttingDown:
     case Dead:
     default:
         break;
     };
 }

 void TasksView::saveGuiState()
 {
     Q_ASSERT( m_ui->treeView );
     ViewFilter* filter = Application::instance().model().taskModel();
     Q_ASSERT( filter );
     QSettings settings;
     // save user settings
     if ( Application::instance().state() == Connected ||
          Application::instance().state() == Disconnecting ) {
         GUIState state;
         // selected task
         state.setSelectedTask( selectedTask().id() );
         // expanded tasks
         TaskList tasks = MODEL.charmDataModel()->getAllTasks();
         TaskIdList expandedTasks;
         Q_FOREACH( Task task, tasks ) {
             QModelIndex index( filter->indexForTaskId( task.id() ) );
             if ( m_ui->treeView->isExpanded( index ) ) {
                 expandedTasks << task.id();
             }
         }
         state.setExpandedTasks( expandedTasks );
         state.saveTo( settings );
     }
 }

 void TasksView::restoreGuiState()
 {
     Q_ASSERT( m_ui->treeView );
     ViewFilter* filter = Application::instance().model().taskModel();
     Q_ASSERT( filter );
     QSettings settings;
     // restore user settings, but only when we are connected
     // (otherwise, we do not have any user data):
     if ( Application::instance().state() == Connected ) {
         GUIState state;
         state.loadFrom( settings );
         QModelIndex index( filter->indexForTaskId( state.selectedTask() ) );
         if ( index.isValid() ) {
             m_ui->treeView->setCurrentIndex(index);
         }

         Q_FOREACH( TaskId id, state.expandedTasks() ) {
             QModelIndex index( filter->indexForTaskId( id ) );
             if ( index.isValid() ) {
                 m_ui->treeView->expand( index );
             }
         }
     }
 }

 QFont TasksView::configuredFont()
 {
//    QTreeView treeView; // temp, to get default treeView font
     QFont font = QTreeView().font();

     switch( CONFIGURATION.taskTrackerFontSize ) {
     case Configuration::TaskTrackerFont_Small:
         font.setPointSizeF( 0.9 * font.pointSize() );
         break;
     case Configuration::TaskTrackerFont_Regular:
         break;
     case Configuration::TaskTrackerFont_Large:
         font.setPointSizeF( 1.2 * font.pointSize() );
         break;
     default:
         break;
     };
     return font;
 }

 void TasksView::configurationChanged()
 {
     Q_ASSERT( CONFIGURATION.taskPrefilteringMode >= 0 && CONFIGURATION.taskPrefilteringMode < m_ui->tasksCombo->count() );
     m_ui->tasksCombo->setCurrentIndex( CONFIGURATION.taskPrefilteringMode );

     m_ui->treeView->setFont( configuredFont() );
     m_ui->treeView->header()->hide();
     configureUi();
 }

 void TasksView::setModel( ModelConnector* connector )
 {
     Q_ASSERT( m_ui );
     m_ui->treeView->setModel( connector->taskModel() );
 }

 void TasksView::slotFiltertextChanged( const QString& filtertextRaw )
 {
     ViewFilter* filter = Application::instance().model().taskModel();
     QString filtertext = filtertextRaw.simplified();
     filtertext.replace( ' ', '*' );
     filter->setFilterWildcard( filtertext );

     m_ui->buttonClearFilter->setEnabled( ! filtertextRaw.isEmpty() );
     if ( ! filtertextRaw.isEmpty() ) m_ui->treeView->expandAll();
 }

 void TasksView::taskPrefilteringChanged( int index )
 {
     ViewFilter* filter = Application::instance().model().taskModel();
     if ( index >= 0 && index < Configuration::TaskPrefilter_NumberOfModes ) {
         const Configuration::TaskPrefilteringMode mode = static_cast<Configuration::TaskPrefilteringMode>( index );
         filter->setTaskPrefilteringMode( mode );
         CONFIGURATION.taskPrefilteringMode = mode;
         emit saveConfiguration();
     }
 }

 void TasksView::slotContextMenuRequested( const QPoint& point )
 {
     // prepare the menu:
     QMenu menu( m_ui->treeView );
     menu.addAction( &m_actionStartEvent );
     menu.addAction( &m_actionEndEvent );

     menu.addSeparator();
     menu.addAction( &m_actionNewTask );
     menu.addAction( &m_actionNewSubTask );
     menu.addAction( &m_actionEditTask );
     menu.addAction( &m_actionDeleteTask );

     configureUi();

     menu.exec( m_ui->treeView->mapToGlobal( point ) );
 }

 void TasksView::slotItemDoubleClicked( const QModelIndex& index )
 {
     ViewFilter* filter = Application::instance().model().taskModel();

     if ( !index.isValid() ) return;
     if ( index.column() == Column_TaskId ) {
         Task task = filter->taskForIndex( index );
         if ( CONFIGURATION.eventsInLeafsOnly && filter->taskHasChildren( task ) ) {
             return;
         }

         CharmDataModel* model = MODEL.charmDataModel();
         if ( filter->taskIsActive( task ) ) {
             model->endEventRequested( task );
         } else {
             if( task.isCurrentlyValid() ) {
                 model->startEventRequested( task );
             }
         }
     }
     configureUi();
 }

 void TasksView::commitCommand( CharmCommand* command )
 {
     command->finalize();
     delete command;
 }

 void TasksView::slotEventActivated( EventId )
 {
     configureUi();
 }

 void TasksView::slotEventDeactivated( EventId )
 {
     configureUi();
 }

 Task TasksView::selectedTask()
 {
     Q_ASSERT( m_ui->treeView );
     ViewFilter* filter = Application::instance().model().taskModel();
     Q_ASSERT( filter );
     // find current selection
     QItemSelection selection = m_ui->treeView->selectionModel()->selection();
     // match it to a task:
     if ( selection.size() > 0 ) {
         return filter->taskForIndex( selection.indexes().first() );
     } else {
         return Task();
     }
 }

#include "TasksView.moc"
