/*
  EventView.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Frank Osterfeld <frank.osterfeld@kdab.com>

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

#include "EventView.h"
#include "ApplicationCore.h"
#include "Data.h"
#include "EventEditor.h"
#include "EventEditorDelegate.h"
#include "EventModelFilter.h"
#include "FindAndReplaceEventsDialog.h"
#include "MessageBox.h"
#include "SelectTaskDialog.h"
#include "TasksView.h"
#include "ViewHelpers.h"
#include "WeeklyTimesheet.h"

#include "Commands/CommandDeleteEvent.h"
#include "Commands/CommandMakeEvent.h"
#include "Commands/CommandModifyEvent.h"

#include "Core/CharmConstants.h"
#include "Core/CharmDataModel.h"
#include "Core/Configuration.h"
#include "Core/Event.h"
#include "Core/TaskTreeItem.h"

#include <QCloseEvent>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QLabel>
#include <QListView>
#include <QMenu>
#include <QTimer>
#include <QToolBar>
#include <QVBoxLayout>

EventView::EventView( QToolBar* toolBar, QWidget* (parent) )
    : QWidget( parent )
    , m_model( nullptr )
    , m_actionUndo( this )
    , m_actionRedo( this )
    , m_actionNewEvent( this )
    , m_actionEditEvent( this )
    , m_actionDeleteEvent( this )
    , m_actionCreateTimeSheet( this )
    , m_actionFindAndReplace( this )
    , m_comboBox( new QComboBox( this ) )
    , m_labelTotal( new QLabel( this ) )
    , m_listView( new QListView( this ) )
{
    auto layout = new QVBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->addWidget( m_listView );

    m_listView->setAlternatingRowColors( true );
    m_listView->setContextMenuPolicy( Qt::CustomContextMenu );
    connect( m_listView,
             SIGNAL(customContextMenuRequested(QPoint)),
             SLOT(slotContextMenuRequested(QPoint)) );
    connect( m_listView,
             SIGNAL(doubleClicked(QModelIndex)),
             SLOT(slotEventDoubleClicked(QModelIndex)) );
    connect( &m_actionNewEvent, SIGNAL(triggered()),
             SLOT(slotNewEvent()) );
    connect( &m_actionEditEvent, SIGNAL(triggered()),
             SLOT(slotEditEvent()) );
    connect( &m_actionDeleteEvent, SIGNAL(triggered()),
             SLOT(slotDeleteEvent()) );
//     connect( &m_commitTimer, SIGNAL(timeout()),
//              SLOT(slotCommitTimeout()) );
//     m_commitTimer.setSingleShot( true );

    m_actionUndo.setText(tr("Undo"));
    m_actionUndo.setToolTip(tr("Undo the latest change"));
    m_actionUndo.setShortcut(QKeySequence::Undo);
    m_actionUndo.setEnabled(false);

    m_actionRedo.setText(tr("Redo"));
    m_actionRedo.setToolTip(tr("Redo the last undone change."));
    m_actionRedo.setShortcut(QKeySequence::Redo);
    m_actionRedo.setEnabled(false);

    m_undoStack = new QUndoStack(this);
    connect(m_undoStack, SIGNAL(canUndoChanged(bool)), &m_actionUndo, SLOT(setEnabled(bool)));
    connect(m_undoStack, SIGNAL(undoTextChanged(QString)), this, SLOT(slotUndoTextChanged(QString)));
    connect(&m_actionUndo, SIGNAL(triggered()), m_undoStack, SLOT(undo()));

    connect(m_undoStack, SIGNAL(canRedoChanged(bool)), &m_actionRedo, SLOT(setEnabled(bool)));
    connect(m_undoStack, SIGNAL(redoTextChanged(QString)), this, SLOT(slotRedoTextChanged(QString)));
    connect(&m_actionRedo, SIGNAL(triggered()), m_undoStack, SLOT(redo()));

    m_actionNewEvent.setText( tr( "New Event..." ) );
    m_actionNewEvent.setToolTip( tr( "Create a new Event" ) );
    m_actionNewEvent.setIcon( Data::newTaskIcon() );
    m_actionNewEvent.setShortcut( QKeySequence::New );
    toolBar->addAction( &m_actionNewEvent );

    m_actionEditEvent.setText( tr( "Edit Event...") );
    m_actionEditEvent.setShortcut( Qt::CTRL + Qt::Key_E );
    m_actionEditEvent.setIcon( Data::editEventIcon() );
    toolBar->addAction( &m_actionEditEvent );

    m_actionFindAndReplace.setText( tr( "Search/Replace Events..." ) );
    m_actionFindAndReplace.setToolTip( tr( "Change the task events belong to" ) );
    m_actionFindAndReplace.setIcon( Data::searchIcon() );
    toolBar->addAction( &m_actionFindAndReplace );

    connect( &m_actionFindAndReplace, SIGNAL(triggered()), SLOT(slotFindAndReplace()) );

    m_actionDeleteEvent.setText( tr( "Delete Event..." ) );
    QList<QKeySequence> deleteShortcuts;
    deleteShortcuts << QKeySequence::Delete;
#ifdef Q_OS_OSX
    deleteShortcuts << Qt::Key_Backspace;
#endif
    m_actionDeleteEvent.setShortcuts(deleteShortcuts);
    m_actionDeleteEvent.setIcon( Data::deleteTaskIcon() );
    toolBar->addAction( &m_actionDeleteEvent );

    // disable all actions, action state will be set when the current
    // item changes:
    m_actionNewEvent.setEnabled( true ); // always on
    m_actionEditEvent.setEnabled( false );
    m_actionDeleteEvent.setEnabled( false );

    toolBar->addWidget( m_comboBox );
    connect( m_comboBox, SIGNAL(currentIndexChanged(int)),
             SLOT(timeFrameChanged(int)) );

    auto spacer = new QWidget( this );
    QSizePolicy spacerSizePolicy = spacer->sizePolicy();
    spacerSizePolicy.setHorizontalPolicy( QSizePolicy::Expanding );
    spacer->setSizePolicy( spacerSizePolicy );
    toolBar->addWidget( spacer );

    toolBar->addWidget( m_labelTotal );

    QTimer::singleShot( 0, this, SLOT(delayedInitialization()) );

    // I hate doing this but the stupid default view sizeHints suck badly.
    setMinimumHeight( 200 );
}

EventView::~EventView()
{
}

void EventView::delayedInitialization()
{
    timeSpansChanged();
    connect( ApplicationCore::instance().dateChangeWatcher(),
             SIGNAL(dateChanged()),
             SLOT(timeSpansChanged()) );

}

void EventView::populateEditMenu( QMenu* menu )
{
    menu->addAction( &m_actionUndo );
    menu->addAction( &m_actionRedo );
    menu->addSeparator();
    menu->addAction( &m_actionNewEvent );
    menu->addAction( &m_actionEditEvent );
    menu->addAction( &m_actionDeleteEvent );
}

void EventView::timeSpansChanged()
{
    m_timeSpans = TimeSpans( QDate::currentDate() ).standardTimeSpans();
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

void EventView::stageCommand(CharmCommand *command)
{
    auto undoCommand = new UndoCharmCommandWrapper(command);
    connect(command, SIGNAL(emitExecute(CharmCommand*)), this, SIGNAL(emitCommand(CharmCommand*)));
    connect(command, SIGNAL(emitRollback(CharmCommand*)), this, SIGNAL(emitCommandRollback(CharmCommand*)));
    connect(command, SIGNAL(emitSlotEventIdChanged(int,int)), this, SLOT(slotEventIdChanged(int,int)));
    m_undoStack->push(undoCommand);
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
    if ( MessageBox::question(
             this, tr( "Delete Event?" ),
             tr( "<html>Do you really want to delete the event <b>%1</b>?" ).arg(eventDescription),
             tr( "Delete" ), tr("Cancel") )
         == QMessageBox::Yes ) {
        auto command = new CommandDeleteEvent( m_event, this );
        command->prepare();
        stageCommand( command );
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

void EventView::slotUndoTextChanged(const QString &text)
{
    m_actionUndo.setText(tr("Undo %1").arg(text));
}

void EventView::slotRedoTextChanged(const QString &text)
{
    m_actionRedo.setText(tr("Redo %1").arg(text));
}

void EventView::slotEventIdChanged(int oldId, int newId)
{
    foreach(QObject* o, m_undoStack->children()) {
        UndoCharmCommandWrapper* wrapper = dynamic_cast<UndoCharmCommandWrapper*>(o);
        Q_ASSERT(wrapper);
        wrapper->command()->eventIdChanged(oldId, newId);
    }
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

void EventView::slotFindAndReplace()
{
    FindAndReplaceEventsDialog findAndReplace( this );

    if ( findAndReplace.exec() != QDialog::Accepted )
        return;

    QList<Event> events = findAndReplace.modifiedEvents();
    for ( int i = 0; i < events.count(); ++i )
        slotEventChangesCompleted( events[i] );
}

void EventView::slotReset()
{
    timeFrameChanged( m_comboBox->currentIndex() );
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
             SIGNAL(currentChanged(QModelIndex,QModelIndex)),
             SLOT(slotCurrentItemChanged(QModelIndex,QModelIndex)) );

    connect( model, SIGNAL(eventActivationNotice(EventId)),
             SLOT(slotEventActivated(EventId)) );
    connect( model, SIGNAL(eventDeactivationNotice(EventId)),
             SLOT(slotEventDeactivated(EventId)) );

    connect( model, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
             SLOT(slotUpdateCurrent()) );
    connect( model, SIGNAL(rowsInserted(QModelIndex,int,int)),
             SLOT(slotUpdateTotal()) );
    connect( model, SIGNAL(rowsRemoved(QModelIndex,int,int)),
             SLOT(slotUpdateTotal()) );
    connect( model, SIGNAL(rowsInserted(QModelIndex,int,int)),
             SLOT(slotConfigureUi()) );
    connect( model, SIGNAL(rowsRemoved(QModelIndex,int,int)),
             SLOT(slotConfigureUi()) );
    connect( model, SIGNAL(layoutChanged()),
             SLOT(slotUpdateCurrent()) );
    connect( model, SIGNAL(modelReset()),
             SLOT(slotUpdateTotal()) );

    m_model = model;
    // normally, the model is set only once, so this should be no problem:
    auto delegate = new EventEditorDelegate( model, m_listView );
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
        Event newEvent = editor.eventResult();
        if ( !newEvent.isValid() ) {
            auto command =
                new CommandMakeEvent( newEvent, this );
            connect( command, SIGNAL(finishedOk(Event)),
                     this, SLOT(slotEventChangesCompleted(Event)),
                     Qt::QueuedConnection );
            stageCommand( command );
            return;
        } else {
            auto command = new CommandModifyEvent( newEvent, event, this );
            stageCommand( command );
        }
    }
}

void EventView::slotEventChangesCompleted( const Event& event )
{
    // make event editor finished, bypass the undo stack to set its contents
    // undo will just target CommandMakeEvent instead
    auto command = new CommandModifyEvent( event, event, this );
    emitCommand( command );
    delete command;
}

#include "moc_EventView.cpp"
