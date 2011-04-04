#include <QMenu>
#include <QtDebug>
#include <QListView>
#include <QCloseEvent>
#include <QMessageBox>
#include <QDateTimeEdit>
#include <QToolBar>
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>

#include "TasksView.h"
#include "ViewHelpers.h"
#include "Data.h"
#include "Core/Event.h"
#include "Core/CharmConstants.h"
#include "Application.h"
#include "EventView.h"
#include "EventEditor.h"
#include "Core/Configuration.h"
#include "EventEditorDelegate.h"
#include "Core/TaskTreeItem.h"
#include "Core/CharmDataModel.h"
#include "SelectTaskDialog.h"
#include "EventModelFilter.h"
#include "Commands/CommandMakeEvent.h"
#include "Commands/CommandModifyEvent.h"
#include "Commands/CommandDeleteEvent.h"
#include "Reports/WeeklyTimeSheet.h"

EventView::EventView( QToolBar* toolBar, QWidget* parent )
    : QWidget( parent )
    , m_model( 0 )
    , m_actionNewEvent( this )
    , m_actionEditEvent( this )
    , m_actionDeleteEvent( this )
    , m_actionCreateTimeSheet( this )
    , m_comboBox( new QComboBox( this ) )
    , m_labelTotal( new QLabel( this ) )
    , m_listView( new QListView( this ) )
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->addWidget( m_listView );

    m_listView->setAlternatingRowColors( true );
    m_listView->setContextMenuPolicy( Qt::CustomContextMenu );
    connect( m_listView,
             SIGNAL( customContextMenuRequested( const QPoint& ) ),
             SLOT( slotContextMenuRequested( const QPoint& ) ) );
    connect( m_listView,
             SIGNAL( doubleClicked( const QModelIndex& ) ),
             SLOT( slotEventDoubleClicked( const QModelIndex& ) ) );
    connect( &m_actionNewEvent, SIGNAL( triggered() ),
             SLOT( slotNewEvent() ) );
    connect( &m_actionEditEvent, SIGNAL( triggered() ),
             SLOT( slotEditEvent() ) );
    connect( &m_actionDeleteEvent, SIGNAL( triggered() ),
             SLOT( slotDeleteEvent() ) );
    connect( &m_actionCreateTimeSheet, SIGNAL( triggered() ),
             SLOT( slotCreateTimeSheet() ) );
//     connect( &m_commitTimer, SIGNAL( timeout() ),
//              SLOT( slotCommitTimeout() ) );
//     m_commitTimer.setSingleShot( true );


    m_actionNewEvent.setText( tr( "Create New Event..." ) );
    m_actionNewEvent.setIcon( Data::newTaskIcon() );
    m_actionNewEvent.setShortcut( QKeySequence::New );
    toolBar->addAction( &m_actionNewEvent );

    m_actionEditEvent.setText( tr( "Edit Event...") );
    m_actionEditEvent.setShortcut( Qt::CTRL + Qt::Key_E );
    m_actionEditEvent.setIcon( Data::editEventIcon() );
    toolBar->addAction( &m_actionEditEvent );

    m_actionDeleteEvent.setText( tr( "Delete Event..." ) );
    QList<QKeySequence> deleteShortcuts;
    deleteShortcuts << QKeySequence::Delete;
#ifdef Q_WS_MAC
    deleteShortcuts << Qt::Key_Backspace;
#endif
    m_actionDeleteEvent.setShortcuts(deleteShortcuts);
    m_actionDeleteEvent.setIcon( Data::deleteTaskIcon() );
    toolBar->addAction( &m_actionDeleteEvent );

    m_actionCreateTimeSheet.setText( tr( "Create Time Sheet..." ) );
    m_actionCreateTimeSheet.setIcon( Data::createReportIcon() );
    toolBar->addAction( &m_actionCreateTimeSheet );

    // disable all actions, action state will be set when the current
    // item changes:
    m_actionNewEvent.setEnabled( true ); // always on
    m_actionEditEvent.setEnabled( false );
    m_actionDeleteEvent.setEnabled( false );

    toolBar->addWidget( new QLabel( tr( "See events from:" ) , this ) );

    toolBar->addWidget( m_comboBox );
    connect( m_comboBox, SIGNAL( currentIndexChanged( int ) ),
             SLOT( timeFrameChanged( int ) ) );

    QWidget *spacer = new QWidget( this );
    QSizePolicy spacerSizePolicy = spacer->sizePolicy();
    spacerSizePolicy.setHorizontalPolicy( QSizePolicy::Expanding );
    spacer->setSizePolicy( spacerSizePolicy );
    toolBar->addWidget( spacer );

    toolBar->addWidget( m_labelTotal );

    QTimer::singleShot( 0, this, SLOT( delayedInitialization() ) );

    // I hate doing this but the stupid default view sizeHints suck badly.
    setMinimumHeight( 200 );
}

EventView::~EventView()
{
}

void EventView::delayedInitialization()
{
    timeSpansChanged();
    connect( &Application::instance().timeSpans(),
             SIGNAL( timeSpansChanged() ),
             SLOT( timeSpansChanged() ) );

}

void EventView::populateEditMenu( QMenu* menu )
{
    menu->addAction( &m_actionNewEvent );
    menu->addAction( &m_actionEditEvent );
    menu->addAction( &m_actionDeleteEvent );
}

void EventView::timeSpansChanged()
{
    m_timeSpans = Application::instance().timeSpans().standardTimeSpans();
    // close enough to "ever" for our purposes:
    NamedTimeSpan allEvents = {
        tr( "Ever" ),
        TimeSpan( QDate::currentDate().addYears( -200 ),
                  QDate::currentDate().addYears( +200 ) )
    };
    m_timeSpans << allEvents;

    const int currentIndex = m_comboBox->currentIndex();
    m_comboBox->clear();
    for ( int i = 0; i < m_timeSpans.size(); ++i )
    {
        m_comboBox->addItem( m_timeSpans[i].name );
    }
    if ( currentIndex >= 0 &&  currentIndex <= m_timeSpans.size() ) {
        m_comboBox->setCurrentIndex( currentIndex );
    } else {
        m_comboBox->setCurrentIndex( 0 );
    }
}

void EventView::closeEvent( QCloseEvent* e )
{
    e->setAccepted( false );
    reject();
}

void EventView::reject()
{
    emit visible( false );
}

void EventView::commitCommand( CharmCommand* command )
{
    command->finalize();
    delete command;
}

void EventView::slotCurrentItemChanged( const QModelIndex& start,
                                          const QModelIndex& end )
{
    if ( ! start.isValid() ) {
        m_event = Event();
        m_actionDeleteEvent.setEnabled(false);
        m_actionEditEvent.setEnabled(false);
    } else {
        m_actionDeleteEvent.setEnabled(true);
        m_actionEditEvent.setEnabled(true);
        Event event = m_model->eventForIndex( start );
        Q_ASSERT( event.isValid() ); // index is valid,  so...
        setCurrentEvent( event );
    }

    slotConfigureUi();
}

void EventView::setCurrentEvent( const Event& event )
{
    m_event = event;
}

void EventView::slotNewEvent()
{
    SelectTaskDialog dialog( this );
    if ( dialog.exec() ) {
        const TaskTreeItem& item =
            MODEL.charmDataModel()->taskTreeItem( dialog.selectedTask() );
        if ( item.task().isValid() ) {
            Event e;
            e.setTaskId( dialog.selectedTask( ) );
            slotEditEvent( e );
        }
    }
}

void EventView::slotDeleteEvent()
{
    const TaskTreeItem& taskTreeItem =
        MODEL.charmDataModel()->taskTreeItem( m_event.taskId() );
    const QString name = MODEL.charmDataModel()->fullTaskName( taskTreeItem.task() );
    const QDate date = m_event.startDateTime().date();
    const QTime time = m_event.startDateTime().time();
    const QString dateAndDuration = date.toString( Qt::SystemLocaleDate )
           + ' ' + time.toString( Qt::SystemLocaleDate )
           + ' ' + hoursAndMinutes( m_event.duration() );
    const QString eventDescription = name + ' ' + dateAndDuration;
    if ( QMessageBox::question(
             this, tr( "Delete Event?" ),
             tr( "<html>Do you really want to delete the event <b>%1</b>?" ).arg(eventDescription),
             QMessageBox::Ok | QMessageBox::Cancel,
             QMessageBox::Ok )
         == QMessageBox::Ok ) {
        CommandDeleteEvent* command = new CommandDeleteEvent( m_event, this );
        command->prepare();
        emitCommand( command );
    }
}

void EventView::slotPreviousEvent()
{
    const QModelIndex& index = m_model->indexForEvent( m_event );
    Q_ASSERT( index.isValid() && index.row() > 0 && index.row() < m_model->rowCount() );
    const QModelIndex& previousIndex = m_model->index( index.row() - 1, 0, QModelIndex() );
    m_listView->selectionModel()->setCurrentIndex
        ( previousIndex, QItemSelectionModel::ClearAndSelect );
}

void EventView::slotNextEvent()
{
    const QModelIndex& index = m_model->indexForEvent( m_event );
    Q_ASSERT( index.isValid() && index.row() >= 0 && index.row() < m_model->rowCount() - 1 );
    const QModelIndex& nextIndex = m_model->index( index.row() + 1, 0, QModelIndex() );
    m_listView->selectionModel()->setCurrentIndex
        ( nextIndex, QItemSelectionModel::ClearAndSelect );
}

void EventView::slotContextMenuRequested( const QPoint& point )
{
    // prepare the menu:
    QMenu menu( m_listView );
    menu.addAction( &m_actionNewEvent );
    menu.addAction( &m_actionEditEvent );
    menu.addAction( &m_actionDeleteEvent );

    // all actions are handled in their own slots:
    menu.exec( m_listView->mapToGlobal( point ) );
}

// FIXME obsolete
Event EventView::newSettings()
{
    Event event( m_event );
    return event;
}

void EventView::makeVisibleAndCurrent( const Event& event )
{
    // make sure the event filter time span includes the events start
    // time (otherwise it is not visible):
    // (how?: if the event is not in the timespan, expand the timespan
    // as much as needed)
    const int CurrentTimeSpan = m_comboBox->currentIndex();

    if ( ! m_timeSpans[CurrentTimeSpan].contains( event.startDateTime().date() ) ) {
        for ( int i = 0; i < m_timeSpans.size(); ++i )
        {   // at least "ever"  should contain it
            if ( m_timeSpans[i].contains( event.startDateTime().date() ) ) {
                m_comboBox->setCurrentIndex( i );
                break;
            }
        }
    }
    // get an index for the event, and make it the current index:
    const QModelIndex& index = m_model->indexForEvent( event );
    Q_ASSERT( index.isValid() );
    m_listView->selectionModel()->setCurrentIndex
        ( index, QItemSelectionModel::ClearAndSelect );
}

void EventView::timeFrameChanged( int index )
{
    // wait for the next update, in this case:
    if ( m_comboBox->count() == 0 ) return;
    if ( !m_model ) return;
    if ( index >= 0 && index < m_timeSpans.size() ) {
        m_model->setFilterStartDate( m_timeSpans[index].timespan.first );
        m_model->setFilterEndDate( m_timeSpans[index].timespan.second );
    } else {
        Q_ASSERT( false );
    }
}

void EventView::slotEventActivated( EventId )
{
    slotConfigureUi();
}

void EventView::slotEventDeactivated( EventId )
{
    slotConfigureUi();
}

void EventView::slotConfigureUi()
{
    // what a fricking hack - but QDateTimeEdit does not seem to have
    // a simple function to toggle 12h and 24h mode:
    static QString OriginalDateTimeFormat;
    if ( OriginalDateTimeFormat.isEmpty() ) {
        QDateTimeEdit edit( this );
        OriginalDateTimeFormat = edit.displayFormat();
    } // yeah, I know, this will survive changes in the user prefs

    bool active = MODEL.charmDataModel()->isEventActive( m_event.id() );

    m_actionNewEvent.setEnabled( true ); // always on
    m_actionEditEvent.setEnabled( m_event.isValid() );
    m_actionDeleteEvent.setEnabled( m_event.isValid() && ! active );
    // m_ui->frame->setEnabled( ! active );
}

void EventView::slotUpdateCurrent()
{
    Event event = DATAMODEL->eventForId( m_event.id() );
    if ( event != m_event ) {
        setCurrentEvent( event );
    }
    slotUpdateTotal();
}

void EventView::slotCreateTimeSheet()
{
    const int index = m_comboBox->currentIndex();
    const TimeSpan span = m_timeSpans[index].timespan;
    WeeklyTimeSheetReport *timeSheet = new WeeklyTimeSheetReport( this );
    timeSheet->setReportProperties( span.first, span.second, 0, true );
    timeSheet->exec();
    // Deletes itself on close.
}

void EventView::slotUpdateTotal()
{   // what matching signal does the proxy emit?
    int seconds = m_model->totalDuration();
    if ( seconds == 0 ) {
        m_labelTotal->clear();
    } else {
        QString total;
        QTextStream stream( &total );
        stream << "(" << hoursAndMinutes( seconds ) << " total)";
        m_labelTotal->setText( total );
    }
}

// ViewModeInterface:
void EventView::saveGuiState() {}
void EventView::restoreGuiState() {}
void EventView::stateChanged( State previous ) {}

void EventView::configurationChanged()
{
    slotConfigureUi();
}

void EventView::setModel( ModelConnector* connector )
{
    EventModelFilter* model = connector->eventModel();
    m_listView->setModel( model );
    m_listView->setSelectionBehavior( QAbstractItemView::SelectRows );
    m_listView->setSelectionMode( QAbstractItemView::SingleSelection );

    connect( m_listView->selectionModel(),
             SIGNAL( currentChanged( const QModelIndex&, const QModelIndex& ) ),
             SLOT( slotCurrentItemChanged( const QModelIndex&, const QModelIndex& ) ) );

    connect( model, SIGNAL( eventActivationNotice( EventId ) ),
             SLOT( slotEventActivated( EventId ) ) );
    connect( model, SIGNAL( eventDeactivationNotice( EventId ) ),
             SLOT( slotEventDeactivated( EventId ) ) );

    connect( model, SIGNAL( dataChanged( const QModelIndex&, const QModelIndex& ) ),
             SLOT( slotUpdateCurrent() ) );
    connect( model, SIGNAL( rowsInserted( const QModelIndex&, int, int ) ),
             SLOT( slotUpdateTotal() ) );
    connect( model, SIGNAL( rowsRemoved( const QModelIndex&, int, int ) ),
             SLOT( slotUpdateTotal() ) );
    connect( model, SIGNAL( rowsInserted( const QModelIndex&, int, int ) ),
             SLOT( slotConfigureUi() ) );
    connect( model, SIGNAL( rowsRemoved( const QModelIndex&, int, int ) ),
             SLOT( slotConfigureUi() ) );
    connect( model, SIGNAL( layoutChanged() ),
             SLOT( slotUpdateCurrent() ) );
    connect( model, SIGNAL( modelReset() ),
             SLOT( slotUpdateTotal() ) );

    m_model = model;
    // normally, the model is set only once, so this should be no problem:
    EventEditorDelegate* delegate =
        new EventEditorDelegate( model, m_listView );
    m_listView->setItemDelegate( delegate );
    timeSpansChanged();
}

void EventView::slotEventDoubleClicked( const QModelIndex& index )
{
    Q_ASSERT( m_model ); // otherwise, how can we get a doubleclick on an item?
    const Event& event = m_model->eventForIndex( index );
    slotEditEvent( event );
}

void EventView::slotEditEvent()
{
    slotEditEvent( m_event );
}

void EventView::slotEditEvent( const Event& event )
{
    EventEditor editor( event, this );
    if( editor.exec() ) {
        Event newEvent = editor.event();
        if ( !newEvent.isValid() ) {
            CommandMakeEvent* command =
                new CommandMakeEvent( newEvent, this );
            connect( command, SIGNAL( finishedOk( const Event& ) ),
                     this, SLOT( slotEditEventCompleted( const Event& ) ),
                     Qt::QueuedConnection );
            emitCommand( command );
            return;

        } else {
            slotEditEventCompleted( newEvent );
        }
    }
}

void EventView::slotEditEventCompleted( const Event& event )
{
    CommandModifyEvent* command =
        new CommandModifyEvent( event, this );
    emitCommand( command );
}


#include "EventView.moc"
