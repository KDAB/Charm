#include <QtDebug>
#include <QSettings>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QHeaderView>

#include <Core/CharmConstants.h>

#include "ViewHelpers.h"
#include "Data.h"
#include "GUIState.h"
#include "ModelConnector.h"
#include "ViewFilter.h"
#include "Application.h"
#include "ModelConnector.h"
#include "TaskModelAdapter.h"
#include "ViewFilter.h"
#include "TasksView.h"
#include "SelectTaskDialog.h"
#include "ui_SelectTaskDialog.h"

SelectTaskDialogProxy::SelectTaskDialogProxy( CharmDataModel* model, QObject* parent )
    : ViewFilter( model, parent )
{
    // we filter for the task name column
    setFilterKeyColumn( Column_TaskId );
    setFilterCaseSensitivity( Qt::CaseInsensitive );
    setFilterRole( TasksViewRole_Filter );
    prefilteringModeChanged();
}

bool SelectTaskDialogProxy::filterAcceptsColumn( int column, const QModelIndex& parent ) const
{
    return column == Column_TaskId;
}

Qt::ItemFlags SelectTaskDialogProxy::flags( const QModelIndex & index ) const
{
    if ( index.isValid() )
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    else
        return Qt::NoItemFlags;
}

QVariant SelectTaskDialogProxy::data( const QModelIndex& index, int role ) const
{
    if ( index.isValid() && role == Qt::CheckStateRole ) {
        return QVariant();
    } else {
        return ViewFilter::data( index, role );
    }
}

SelectTaskDialog::SelectTaskDialog( QWidget* parent )
    : QDialog( parent )
    , m_ui( new Ui::SelectTaskDialog() )
    , m_proxy( MODEL.charmDataModel() )
{
    m_ui->setupUi( this );
    m_ui->treeView->setModel( &m_proxy );
    m_ui->treeView->expandAll();
    m_ui->treeView->header()->hide();
    connect( m_ui->treeView->selectionModel(),
             SIGNAL( currentChanged( const QModelIndex&, const QModelIndex& ) ),
             SLOT( slotCurrentItemChanged( const QModelIndex&, const QModelIndex& ) ) );
    connect( m_ui->treeView,
             SIGNAL( doubleClicked( const QModelIndex& ) ),
             SLOT( slotDoubleClicked( const QModelIndex& ) ) );

    connect( m_ui->filter, SIGNAL( textChanged( QString ) ),
             SLOT( slotFilterTextChanged( QString ) ) );
    connect( this, SIGNAL( accepted() ),
    		 SLOT( slotAccepted() ) );

    QSettings settings;
    settings.beginGroup( staticMetaObject.className() );
    if ( settings.contains( MetaKey_MainWindowGeometry ) ) {
        resize( settings.value( MetaKey_MainWindowGeometry ).toSize() );
    }

    m_ui->filter->setFocus();
}

SelectTaskDialog::~SelectTaskDialog()
{
	delete m_ui; m_ui = 0;
}

void SelectTaskDialog::showEvent ( QShowEvent * event )
{
    QSettings settings;
    settings.beginGroup( staticMetaObject.className() );
    GUIState state;
    state.loadFrom( settings );
    QModelIndex index( m_proxy.indexForTaskId( state.selectedTask() ) );
    if ( index.isValid() ) {
        m_ui->treeView->setCurrentIndex(index);
    }

    Q_FOREACH( const TaskId id, state.expandedTasks() ) {
        QModelIndex indexForId( m_proxy.indexForTaskId( id ) );
        if ( indexForId.isValid() ) {
            m_ui->treeView->expand( indexForId );
        }
    }

    QDialog::showEvent( event );
}

void SelectTaskDialog::hideEvent( QHideEvent* event )
{
    QSettings settings;
    settings.beginGroup( staticMetaObject.className() );
    settings.setValue( MetaKey_MainWindowGeometry, size() );
    QDialog::hideEvent( event );
}

TaskId SelectTaskDialog::selectedTask() const
{
    return m_selectedTask;
}

void SelectTaskDialog::slotCurrentItemChanged( const QModelIndex& first,
                                               const QModelIndex& )
{
    if ( isValidAndTrackable( first ) ) {
        const Task task = m_proxy.taskForIndex( first );
        m_selectedTask = task.id();
    } else {
        m_selectedTask = 0;
    }
    m_ui->buttonBox->button( QDialogButtonBox::Ok )->setEnabled( m_selectedTask != 0 );
}


bool SelectTaskDialog::isValidAndTrackable( const QModelIndex& index ) const
{
    if ( !index.isValid() )
        return false;
    const Task task = m_proxy.taskForIndex( index );
    return task.isValid() && task.trackable();
}

void SelectTaskDialog::slotDoubleClicked ( const QModelIndex & index )
{
    if ( isValidAndTrackable( index ) ) {
        accept();
    }
}


void SelectTaskDialog::slotFilterTextChanged( const QString& text )
{
    QString filtertext = text.simplified();
    filtertext.replace( ' ', '*' );

    Charm::saveExpandStates( m_ui->treeView, &m_expansionStates );
    m_proxy.setFilterWildcard( filtertext );
    Charm::restoreExpandStates( m_ui->treeView, &m_expansionStates );
}

void SelectTaskDialog::slotAccepted()
{
    QSettings settings;
    // FIXME refactor, code duplication with taskview
    // save user settings
    if ( Application::instance().state() == Connected ||
         Application::instance().state() == Disconnecting ) {
        GUIState state;
        // selected task
        state.setSelectedTask( selectedTask() );
        // expanded tasks
        TaskList tasks = MODEL.charmDataModel()->getAllTasks();
        TaskIdList expandedTasks;
        Q_FOREACH( Task task, tasks ) {
            QModelIndex index( m_proxy.indexForTaskId( task.id() ) );
            if ( m_ui->treeView->isExpanded( index ) ) {
                expandedTasks << task.id();
            }
        }
        state.setExpandedTasks( expandedTasks );
        settings.beginGroup( staticMetaObject.className() );
        state.saveTo( settings );
    }
}

#include "SelectTaskDialog.moc"

