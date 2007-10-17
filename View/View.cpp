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
#include "View.h"
#include "ViewHelpers.h"
#include "Core/State.h"
#include "GUIState.h"
#include "ViewFilter.h"
#include "StatusBarWidget.h"
#include "Application.h"
#include "TaskIdDialog.h"
#include "CharmPreferences.h"
#include "ChattyItemDelegate.h"
#include "CharmAboutDialog.h"

#include "Core/CharmCommand.h"
#include "Commands/CommandRelayCommand.h"
#include "Commands/CommandAddTask.h"
#include "Commands/CommandDeleteTask.h"

#include "ui_View.h"

View::View( QWidget* parent )
    : QMainWindow( parent )
    , ViewInterface()
    , m_ui( new Ui::View )
    , m_delegate( new ChattyItemDelegate( this ) )
    , m_actionEventStarted( this )
    , m_actionEventEnded( this )
    , m_actionSelectedEventStarted( this )
    , m_actionSelectedEventEnded( this )
    , m_actionNewTask( this )
    , m_actionNewSubTask( this )
    , m_actionDeleteTask( this )
    , m_actionQuit( this )
    , m_actionPreferences( this )
    , m_actionReporting( this )
    , m_actionEventEditor( this )
    , m_actionAboutDialog( this )
    , m_actionShowHideView( this )
    , m_actionStopAllTasks( this )
    , m_eventEditor( this )
    , m_reportDialog( this )
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
    m_actionAboutDialog.setText( tr( "About Charm" ) );
    connect( &m_actionAboutDialog, SIGNAL( triggered() ),
             SLOT( slotAboutDialog() ) );
    connect( &m_actionShowHideView, SIGNAL( triggered() ),
             SLOT( slotShowHideView() ) );
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

    m_actionQuit.setText( tr( "Quit" ) );
    m_actionQuit.setIcon( Data::quitCharmIcon() );
    connect( &m_actionQuit, SIGNAL( triggered( bool ) ),
             SLOT( slotQuit() ) );

    m_actionPreferences.setText( tr( "Preferences" ) );
    m_actionPreferences.setIcon( Data::configureIcon() );
    connect( &m_actionPreferences, SIGNAL( triggered( bool ) ),
             SLOT( slotEditPreferences( bool ) ) );

    m_actionReporting.setText( tr( "Reports..." ) );
    m_actionReporting.setCheckable( true );
    m_actionReporting.setChecked( false );
    m_reportDialog.hide();
    connect( &m_actionReporting, SIGNAL( triggered( bool ) ),
             &m_reportDialog, SLOT( setVisible( bool ) ) );
    connect( &m_reportDialog, SIGNAL( visible( bool ) ),
             &m_actionReporting, SLOT( setChecked( bool ) ) );

    m_actionEventEditor.setText( tr( "Event Editor" ) );
    m_actionEventEditor.setCheckable( true );
    m_eventEditor.setVisible( m_actionEventEditor.isChecked() );
    connect( &m_actionEventEditor, SIGNAL( toggled( bool ) ),
             &m_eventEditor, SLOT( setVisible( bool ) ) );
    connect( &m_eventEditor, SIGNAL( visible( bool ) ),
             &m_actionEventEditor, SLOT( setChecked( bool ) ) );
    // setup Charm menu:
    QMenu* appMenu = new QMenu( tr( "Charm" ), menuBar() );
    appMenu->addAction( &m_actionPreferences );
    m_actionPreferences.setEnabled( true );
    appMenu->addAction( &m_actionAboutDialog );
    appMenu->addAction( &m_actionQuit );

    QMenu* viewMenu = new QMenu( tr( "View" ), menuBar() );
    viewMenu->addAction( &m_actionEventEditor );
    viewMenu->addAction( &m_actionReporting );
//     QMenu* taskMenu = new QMenu ( tr( "Task" ), menuBar() );
//     taskMenu->addAction( &m_actionEventStarted );
//     taskMenu->addAction( &m_actionEventEnded );
    menuBar()->addMenu( appMenu );
    menuBar()->addMenu( viewMenu );

    // system tray icon:
    m_trayIcon.setIcon( Data::charmIcon() );
    m_trayIcon.show();
    connect( &m_trayIcon, SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ),
             SLOT( slotTrayIconActivated( QSystemTrayIcon::ActivationReason ) ) );
    m_systrayContextMenu.addAction( &m_actionShowHideView );
    m_systrayContextMenu.addAction( &m_actionStopAllTasks );
    m_systrayContextMenu.addSeparator();
    m_systrayContextMenu.addAction( &m_actionQuit );
    m_trayIcon.setContextMenu( &m_systrayContextMenu );

    // status bar:
    // m_statusBarWidget = new StatusBarWidget( statusBar() );
    // statusBar()->addWidget( m_statusBarWidget, 1 );

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

void View::restore()
{
    show();
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

void View::closeEvent( QCloseEvent* e )
{
    saveGuiState();
    QMainWindow::closeEvent( e );
}

void View::showEvent( QShowEvent* )
{
    restoreGuiState();
    bool on = CONFIGURATION.showOnlySubscribedTasks;
    m_ui->tasksCombo->setCurrentIndex( on == true ? 1 : 0 );
    slotConfigureUi();
    m_actionShowHideView.setText( tr( "Hide All Windows" ) );
}

void View::hideEvent( QHideEvent* )
{
    m_actionShowHideView.setText( tr( "Show Task Window" ) );
}

void View::slotConfigureUi()
{
    configureUi( m_ui->treeView->selectionModel()->currentIndex() );
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

        // set the model for the event editor:
        m_eventEditor.setModel( MODEL.eventModel() );
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
        configurePermanentUiSettings();
        restoreGuiState();
        break;
    case Disconnecting:
        saveGuiState();
        // fallthrough intended
    case ShuttingDown:
    case Dead:
        setEnabled( false );
        break;
    default:
        break;
    };

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
            sendCommand( cmd );
        }
    } else if ( result == &m_actionDeleteTask ) {
        Q_ASSERT( task.isValid() );
        if ( QMessageBox::question( this, tr( "Delete Task?" ),
                                    tr( "Do you really want to delete this task?" ),
                                    QMessageBox::Ok | QMessageBox::Cancel,
                                    QMessageBox::Ok )
             == QMessageBox::Ok ) {
            CommandDeleteTask* cmd = new CommandDeleteTask( task, this );
            sendCommand( cmd );
        }
    }
}

void View::actionNewTask( bool )
{
    // ... nothing done here, but in the context menu method
}

void View::slotEditPreferences( bool )
{
    CharmPreferences dialog( CONFIGURATION, this );

    if ( dialog.exec() ) {
        CONFIGURATION.eventsInLeafsOnly = dialog.eventsInLeafsOnly();
        CONFIGURATION.oneEventAtATime = dialog.oneEventAtATime();
        CONFIGURATION.taskTrackerFontSize = dialog.taskTrackerFontSize();
        CONFIGURATION.always24hEditing = dialog.always24hEditing();
        configurePermanentUiSettings();
        slotConfigureUi();
        emit saveConfiguration();
    }
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

void View::sendCommand( CharmCommand* cmd )
{
    cmd->prepare();
    CommandRelayCommand* relay = new CommandRelayCommand( this );
    relay->setCommand( cmd );
    emit emitCommand( relay );
}

void View::slotEventActivated( EventId )
{
    slotConfigureUi();
}

void View::slotEventDeactivated( EventId )
{
    slotConfigureUi();
}

void View::slotQuit()
{
    // this saves changes:
    m_eventEditor.close();
    m_reportDialog.close();
    emit quit();
}

void View::configurePermanentUiSettings()
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

    m_eventEditor.slotConfigureUi();
}

void View::slotAboutDialog()
{
    CharmAboutDialog dialog( this );
    dialog.exec();
}

void View::slotTrayIconActivated( QSystemTrayIcon::ActivationReason reason )
{
    switch( reason ) {
    case QSystemTrayIcon::Context:
        // show context menu
        // m_contextMenu.show();
        break;
    case QSystemTrayIcon::DoubleClick:
        slotShowHideView();
        break;
    case QSystemTrayIcon::Trigger:
        // single click
        break;
    case QSystemTrayIcon::MiddleClick:
        // ...
        break;
    case QSystemTrayIcon::Unknown:
    default:
        break;
    }
}

void View::slotShowHideView()
{   // restore the view
    if ( isVisible() ) {
        hide();
        m_eventEditor.hide();
        m_reportDialog.hide();
    } else {
        restore();
    }
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


void View::saveGuiState()
{
    Q_ASSERT( m_ui->treeView );
    ViewFilter* filter = Application::instance().model().taskModel();
    Q_ASSERT( filter );
    QSettings settings;
    // save geometry
    settings.setValue( MetaKey_MainWindowGeometry, saveGeometry() );
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

void View::restoreGuiState()
{
    Q_ASSERT( m_ui->treeView );
    ViewFilter* filter = Application::instance().model().taskModel();
    Q_ASSERT( filter );
    // restore geometry
    QSettings settings;
    if ( settings.contains( MetaKey_MainWindowGeometry ) ) {
        restoreGeometry( settings.value( MetaKey_MainWindowGeometry ).toByteArray() );
    }
    // restore user settings, but only when we are connected
    // (otherwise, we do not have any user data):
    if ( Application::instance().state() == Connected ) {
        GUIState state;
        state.loadFrom( settings );
        QModelIndex index( filter->indexForTaskId( state.selectedTask() ) );
        if ( index.isValid() ) {
            m_ui->treeView->selectionModel()->select(
                index, QItemSelectionModel::Select | QItemSelectionModel::Rows );
        }

        Q_FOREACH( TaskId id, state.expandedTasks() ) {
            QModelIndex index( filter->indexForTaskId( id ) );
            if ( index.isValid() ) {
                m_ui->treeView->expand( index );
            }
        }
    }
}

#include "View.moc"
