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
    , m_actionEventStarted( this )
    , m_actionEventEnded( this )
    , m_actionSelectedEventStarted( this )
    , m_actionSelectedEventEnded( this )
    , m_actionNewTask( this )
    , m_actionNewSubTask( this )
    , m_actionEditTask( this )
    , m_actionDeleteTask( this )
{
    m_ui->setupUi( this );
    setWindowIcon( Data::charmIcon() );
    m_ui->treeView->setItemDelegate( m_delegate );
    connect( m_delegate, SIGNAL( editingStateChanged() ),
             SLOT( slotConfigureUi() ) );
    m_ui->buttonClearFilter->setEnabled( false );
    m_ui->buttonClearFilter->setText( tr( "Clear Filter" ) );
    m_ui->buttonClearFilter->setIcon( Data::clearFilterIcon() );
    // set up actions
    // (no menu icons, please) m_actionAboutDialog.setIcon( Data::charmIcon() );
    m_actionEventStarted.setIcon( Data::goIcon() );
    m_actionEventStarted.setText( tr( "Start Task" ) );
    m_actionSelectedEventStarted.setIcon( m_actionEventStarted.icon() );
    m_actionSelectedEventStarted.setText( m_actionEventStarted.text() );
    m_ui->goButton->setDefaultAction( &m_actionSelectedEventStarted );
    connect( &m_actionSelectedEventStarted, SIGNAL( triggered( bool ) ),
             SLOT( actionSelectedEventStarted( bool ) ) );

    m_actionEventEnded.setIcon( Data::stopIcon() );
    m_actionEventEnded.setText( tr( "Stop Task" ) );
    m_actionSelectedEventEnded.setIcon( m_actionEventEnded.icon() );
    m_actionSelectedEventEnded.setText( m_actionEventEnded.text() );
    m_ui->stopButton->setDefaultAction( &m_actionSelectedEventEnded );
    connect( &m_actionSelectedEventEnded, SIGNAL( triggered( bool ) ),
             SLOT( actionSelectedEventEnded( bool ) ) );


    m_actionNewTask.setText( tr( "New &Task" ) );
    m_actionNewTask.setIcon( Data::newTaskIcon() );

    m_actionNewSubTask.setText( tr( "New &Subtask" ) );
    m_actionNewSubTask.setIcon( Data::newTaskIcon() );

    m_actionEditTask.setText( tr( "Edit Task" ) );
    m_actionEditTask.setIcon( Data::editTaskIcon() );

    m_actionDeleteTask.setText( tr( "Delete Task" ) );
    m_actionDeleteTask.setIcon( Data::deleteTaskIcon() );

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

void TasksView::actionSelectedEventStarted( bool b ) // bool triggered
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

void TasksView::actionSelectedEventEnded( bool ) // bool triggered
{
    Task task = selectedTask();
    // emit signal:
    if ( task.isValid() )
    {
        CharmDataModel* model = MODEL.charmDataModel();
        model->endEventRequested( task );
    }
}

void TasksView::viewCurrentChanged( const QModelIndex& current,
                               const QModelIndex& )
{
    configureUi( current );
}

void TasksView::configureUi( const QModelIndex& current )
{
    if ( ! current.isValid() )
    {
        m_actionSelectedEventStarted.setEnabled( false );
        m_actionSelectedEventEnded.setEnabled( false );
        return;
    }

    ViewFilter* filter = Application::instance().model().taskModel();

    bool editing = m_delegate->isEditing();

    // get the currently selected task:
    Task task = filter->taskForIndex( current );
    if ( task.isValid() && filter->taskIsActive( task ) ) {
        if ( editing ) {
            // If the tree view is in editing state, we do not allow
            // to stop the task, because that invalidates the entered
            // comment before it is committed. If the user selects
            // another tree item with the mouse, the editor closes,
            // too. This is not considered that bad, since it requires
            // explicit activity by the user.
            m_actionSelectedEventStarted.setEnabled( false );
            m_actionSelectedEventEnded.setEnabled( false );
        } else {
            m_actionSelectedEventStarted.setEnabled( false );
            m_actionSelectedEventEnded.setEnabled( true );
        }
    } else if ( task.isValid() && ! task.isCurrentlyValid() ) {
    	// the task is valid, but at this time not within its valid time frame
        m_actionSelectedEventStarted.setEnabled( false );
        m_actionSelectedEventEnded.setEnabled( false );
    } else if ( task.isValid() ) {
        m_actionSelectedEventStarted.setEnabled( true );
        m_actionSelectedEventEnded.setEnabled( false );
    } else {
        m_actionSelectedEventStarted.setEnabled( false );
        m_actionSelectedEventEnded.setEnabled( false );
    }

    if ( filter->taskHasChildren( task ) && CONFIGURATION.eventsInLeafsOnly ) {
        m_actionSelectedEventStarted.setEnabled( false );
    }
}

void TasksView::closeEvent( QCloseEvent* )
{
    saveGuiState();
}

void TasksView::showEvent( QShowEvent* )
{
    restoreGuiState();
}

void TasksView::slotConfigureUi()
{
    if ( QItemSelectionModel* smodel = m_ui->treeView->selectionModel() ) {
        configureUi( smodel->currentIndex() );
    }
}

void TasksView::stateChanged( State previous )
{
    switch( Application::instance().state() ) {
    case Connecting:
    {
        // set model on view:
        ViewFilter* filter = Application::instance().model().taskModel();
        m_ui->treeView->setModel( filter );
        connect( m_ui->treeView->selectionModel(),
                 SIGNAL( currentChanged( const QModelIndex&, const QModelIndex& ) ),
                 SLOT( viewCurrentChanged( const QModelIndex&, const QModelIndex& ) ) );
        connect( filter, SIGNAL( eventActivationNotice( EventId ) ),
                 SLOT( slotEventActivated( EventId ) ) );
        connect( filter, SIGNAL( eventDeactivationNotice( EventId ) ),
                 SLOT( slotEventDeactivated( EventId ) ) );
        configurationChanged();
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
    slotConfigureUi();
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
    ViewFilter* filter = MODEL.taskModel();

    // prepare the menu:
    QMenu menu( m_ui->treeView );
    menu.addAction( &m_actionEventStarted );
    menu.addAction( &m_actionEventEnded );
    menu.addSeparator();
    menu.addAction( &m_actionNewTask );
    menu.addAction( &m_actionNewSubTask );
    menu.addAction( &m_actionEditTask );
    menu.addAction( &m_actionDeleteTask );

    // find the model index at pos:
    QModelIndex currentIndex = m_ui->treeView->indexAt( point );
    bool selected = currentIndex.isValid();
    Task task = filter->taskForIndex( currentIndex );
    bool cannotStart = task.isValid()
		&& filter->taskHasChildren( task )
		&& CONFIGURATION.eventsInLeafsOnly;
	bool cannotStop = m_delegate->isEditing();

    m_actionDeleteTask.setEnabled( task.isValid() && ! filter->taskHasChildren( task ) );
    m_actionEditTask.setEnabled( task.isValid() );
    m_actionNewSubTask.setEnabled( selected );
	m_actionEventStarted.setEnabled( selected && ! cannotStart
			&& ! filter->taskIsActive( task )
			&& task.isCurrentlyValid() );
	m_actionEventEnded.setEnabled( selected && filter->taskIsActive( task ) && ! cannotStop);

    // open the menu:
    QAction* result = menu.exec( m_ui->treeView->mapToGlobal( point ) );
    if (  result == &m_actionEventStarted ) {
        // action triggers it
        if ( m_actionEventStarted.isEnabled() ) {
            actionSelectedEventStarted( false );
        }
    } else if ( result == &m_actionEventEnded ) {
        // action triggers it
        if ( m_actionEventEnded.isEnabled() ) {
            actionSelectedEventEnded( false );
        }
    } else if ( result == &m_actionNewTask || result == &m_actionNewSubTask ) {
        Task newTask;
        int suggestedId = 1;
        if ( selected && result == &m_actionNewSubTask ) {
            newTask.setParent( task.id() );
            // subscribe if the parent is subscribed:
            newTask.setSubscribed( task.subscribed()
                                   || CONFIGURATION.taskPrefilteringMode == Configuration::TaskPrefilter_SubscribedOnly
                                   || CONFIGURATION.taskPrefilteringMode == Configuration::TaskPrefilter_SubscribedAndCurrentOnly );
        }
        // yeah, daredevil!
        while ( filter->taskIdExists( suggestedId ) ) ++suggestedId;
        TaskIdDialog dialog( filter, this );
        dialog.setSuggestedId( suggestedId );
        if ( dialog.exec() ) {
            newTask.setId( dialog.selectedId() );
            newTask.setName( dialog.taskName() );
            CommandAddTask* cmd = new CommandAddTask( newTask, this );
            emit emitCommand( cmd );
        }
    } else if ( result == &m_actionEditTask ) {
    	Q_ASSERT( task.isValid() );
    	TaskEditor editor( this );
    	editor.setTask( task );
    	if( editor.exec() ) {
    		task = editor.getTask();
    		task.dump();
			CommandModifyTask* cmd = new CommandModifyTask( task, this );
			emit emitCommand( cmd );
		}
    	slotConfigureUi();
    } else if ( result == &m_actionDeleteTask ) {
        Q_ASSERT( task.isValid() );
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
    slotConfigureUi();
}

void TasksView::commitCommand( CharmCommand* command )
{
    command->finalize();
    delete command;
}

void TasksView::slotEventActivated( EventId )
{
    slotConfigureUi();
}

void TasksView::slotEventDeactivated( EventId )
{
    slotConfigureUi();
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
