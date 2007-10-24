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
#include "ViewHelpers.h"
#include "Core/State.h"
#include "GUIState.h"
#include "ViewFilter.h"
#include "Application.h"
#include "TaskIdDialog.h"
#include "ChattyItemDelegate.h"

#include "Core/CharmCommand.h"
#include "Commands/CommandRelayCommand.h"
#include "Commands/CommandAddTask.h"
#include "Commands/CommandDeleteTask.h"

#include "ui_TasksView.h"

View::View( QWidget* parent )
    : QWidget( parent )
    // , ViewInterface()
    , m_ui( new Ui::View )
    , m_delegate( new ChattyItemDelegate( this ) )
    , m_actionEventStarted( this )
    , m_actionEventEnded( this )
    , m_actionSelectedEventStarted( this )
    , m_actionSelectedEventEnded( this )
    , m_actionNewTask( this )
    , m_actionNewSubTask( this )
    , m_actionDeleteTask( this )
    , m_actionStopAllTasks( this )
{
    m_ui->setupUi( this );
    setWindowIcon( Data::charmIcon() );
    m_ui->treeView->setItemDelegate( m_delegate );
    connect( m_delegate, SIGNAL( editingStateChanged() ),
             SLOT( slotConfigureUi() ) );
    m_ui->buttonClearFilter->setEnabled( false );
    m_ui->buttonClearFilter->setIcon( Data::clearFilterIcon() );
    // set up actions
    // (no menu icons, please) m_actionAboutDialog.setIcon( Data::charmIcon() );
    m_actionStopAllTasks.setText( tr( "Stop All Active Tasks" ) );
    connect( &m_actionStopAllTasks, SIGNAL( triggered() ),
             SLOT( slotStopAllTasks() ) );
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

    m_actionNewTask.setText( tr( "New Task" ) );
    m_actionNewTask.setIcon( Data::newTaskIcon() );
    connect( &m_actionNewTask, SIGNAL( triggered( bool ) ),
             SLOT( actionNewTask( bool ) ) );
    m_actionNewSubTask.setText( tr( "New Subtask" ) );
    m_actionNewSubTask.setIcon( Data::newTaskIcon() );

    m_actionDeleteTask.setText( tr( "Delete Task" ) );
    m_actionDeleteTask.setIcon( Data::deleteTaskIcon() );

    // filter setup
    connect( m_ui->filterLineEdit, SIGNAL( textChanged( const QString& ) ),
             SLOT( slotFiltertextChanged( const QString& ) ) );
    connect( m_ui->tasksCombo, SIGNAL( currentIndexChanged( int ) ),
             SLOT( subscribedOnlyModeChanged( int ) ) );
    m_ui->treeView->setContextMenuPolicy( Qt::CustomContextMenu );
    connect( m_ui->treeView, SIGNAL( customContextMenuRequested( const QPoint& ) ),
             SLOT( slotContextMenuRequested( const QPoint& ) ) );

    connect( m_ui->treeView, SIGNAL( doubleClicked( const QModelIndex& ) ),
             SLOT( slotItemDoubleClicked( const QModelIndex& ) ) );
    m_ui->tasksCombo->setCurrentIndex( 0 );
}

View::~View()
{
    delete m_ui; m_ui = 0;
}

void View::actionSelectedEventStarted( bool b ) // bool triggered
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

void View::actionSelectedEventEnded( bool ) // bool triggered
{
    Task task = selectedTask();
    // emit signal:
    if ( task.isValid() )
    {
        CharmDataModel* model = MODEL.charmDataModel();
        model->endEventRequested( task );
    }
}

void View::viewCurrentChanged( const QModelIndex& current,
                               const QModelIndex& )
{
    configureUi( current );
}

void View::configureUi( const QModelIndex& current )
{
    // FIXME add state handling
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
    } else {
        m_actionSelectedEventStarted.setEnabled( true );
        m_actionSelectedEventEnded.setEnabled( false );
    }

    if ( filter->taskHasChildren( task ) && CONFIGURATION.eventsInLeafsOnly ) {
        m_actionSelectedEventStarted.setEnabled( false );
    }
}

void View::closeEvent( QCloseEvent* )
{
    saveGuiState();
}

void View::showEvent( QShowEvent* )
{
    restoreGuiState();
    bool on = CONFIGURATION.showOnlySubscribedTasks;
    m_ui->tasksCombo->setCurrentIndex( on == true ? 1 : 0 );
    slotConfigureUi();
}

void View::slotConfigureUi()
{
    if ( QItemSelectionModel* smodel = m_ui->treeView->selectionModel() ) {
        configureUi( smodel->currentIndex() );
    }
}

void View::stateChanged( State previous )
{
    if ( previous == Constructed ) {
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

        // column resize behaviour:
        QHeaderView* header = m_ui->treeView->header();
        header->setResizeMode( Column_TaskId, QHeaderView::ResizeToContents );
        header->setResizeMode( Column_TaskName, QHeaderView::Stretch );
        header->setResizeMode( Column_TaskSubscriptions, QHeaderView::ResizeToContents );
        header->setResizeMode( Column_TaskSessionTime, QHeaderView::ResizeToContents );
        header->setResizeMode( Column_TaskComment, QHeaderView::Stretch );
    }

    switch( Application::instance().state() ) {
    case Connecting:
    {
        bool on =  CONFIGURATION.showOnlySubscribedTasks;
        m_ui->tasksCombo->setCurrentIndex( on == true ? 1 : 0 );
    }
    break;
    case Connected:
        configurationChanged();;
        break;
    case Disconnecting:
    case ShuttingDown:
    case Dead:
    default:
        break;
    };

}

void View::saveGuiState() {}
void View::restoreGuiState() {}
void View::configurationChanged()
{
    QTreeView treeView; // temp, to get default treeView font
    QFont font = treeView.font();

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
    m_ui->treeView->setFont( font );

    slotConfigureUi();
}

void View::setModel( ModelConnector* connector )
{
    Q_ASSERT( m_ui );
    m_ui->treeView->setModel( connector->taskModel() );
}

void View::slotFiltertextChanged( const QString& filtertextRaw )
{
    ViewFilter* filter = Application::instance().model().taskModel();
    QString filtertext = filtertextRaw.simplified();
    filtertext.replace( ' ', '*' );
    filter->setFilterWildcard( filtertext );

    m_ui->buttonClearFilter->setEnabled( ! filtertextRaw.isEmpty() );
    if ( ! filtertextRaw.isEmpty() ) m_ui->treeView->expandAll();
}

void View::subscribedOnlyModeChanged( int index )
{
    ViewFilter* filter = Application::instance().model().taskModel();
    bool on = ( index == 1 );
    filter->setSubscribedTasksOnlyMode( on );
    CONFIGURATION.showOnlySubscribedTasks = on;
    emit saveConfiguration();
}

void View::slotContextMenuRequested( const QPoint& point )
{
    ViewFilter* filter = MODEL.taskModel();

    // prepare the menu:
    QMenu menu( m_ui->treeView );
    menu.addAction( &m_actionEventStarted );
    menu.addAction( &m_actionEventEnded );
    menu.addSeparator();
    menu.addAction( &m_actionNewTask );
    menu.addAction( &m_actionNewSubTask );
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
    m_actionNewSubTask.setEnabled( selected );
    m_actionEventStarted.setEnabled( selected && ! cannotStart
                                     && ! filter->taskIsActive( task ) );
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
                                   || CONFIGURATION.showOnlySubscribedTasks );
        }
        // yeah, daredevil!
        while ( filter->taskIdExists( suggestedId ) ) ++suggestedId;
        TaskIdDialog dialog( filter, this );
        dialog.setSuggestedId( suggestedId );
        if ( dialog.exec() ) {
            newTask.setId( dialog.selectedId() );
            CommandAddTask* cmd = new CommandAddTask( newTask, this );
            emit emitCommand( cmd );
        }
    } else if ( result == &m_actionDeleteTask ) {
        Q_ASSERT( task.isValid() );
        if ( QMessageBox::question( this, tr( "Delete Task?" ),
                                    tr( "Do you really want to delete this task?" ),
                                    QMessageBox::Ok | QMessageBox::Cancel,
                                    QMessageBox::Ok )
             == QMessageBox::Ok ) {
            CommandDeleteTask* cmd = new CommandDeleteTask( task, this );
            emit emitCommand( cmd );
        }
    }
}

void View::actionNewTask( bool )
{
    // ... nothing done here, but in the context menu method
}

void View::slotItemDoubleClicked( const QModelIndex& index )
{
    ViewFilter* filter = Application::instance().model().taskModel();

    if ( !index.isValid() ) return;
    if ( index.column() == Column_TaskSessionTime ) {
        Task task = filter->taskForIndex( index );
        if ( CONFIGURATION.eventsInLeafsOnly && filter->taskHasChildren( task ) ) {
            return;
        }

        CharmDataModel* model = MODEL.charmDataModel();
        if ( filter->taskIsActive( task ) ) {
            model->endEventRequested( task );
        } else {
            model->startEventRequested( task );
        }
    }
    slotConfigureUi();
}

void View::commitCommand( CharmCommand* command )
{
    command->finalize();
    delete command;
}

void View::slotEventActivated( EventId )
{
    slotConfigureUi();
}

void View::slotEventDeactivated( EventId )
{
    slotConfigureUi();
}

void View::slotStopAllTasks()
{
    CharmDataModel* model = MODEL.charmDataModel();
    model->endAllEventsRequested();
}

Task View::selectedTask()
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

QAction* View::actionStopAllTasks()
{
    return &m_actionStopAllTasks;
}

#include "TasksView.moc"
