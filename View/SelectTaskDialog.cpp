#include <QtDebug>
#include <QPushButton>
#include <QDialogButtonBox>

#include "Data.h"
#include "Model.h"
#include "Core/CharmConstants.h"
#include "ViewFilter.h"
#include "Application.h"
#include "Core/CharmDataModel.h"
#include "TaskModelAdapter.h"
#include "ViewFilter.h"
#include "SelectTaskDialog.h"

SelectTaskDialogProxy::SelectTaskDialogProxy()
    : QSortFilterProxyModel()
{
    // we filter for the task name column
    setFilterKeyColumn( Column_TaskName );
    setFilterCaseSensitivity( Qt::CaseInsensitive );
}

QModelIndex SelectTaskDialogProxy::indexForTaskId( TaskId ) const
{
    return QModelIndex(); // not needed
}

bool SelectTaskDialogProxy::filterAcceptsColumn( int column, const QModelIndex& parent ) const
{
    return column == Column_TaskId || column == Column_TaskName;
}

const ViewFilter* SelectTaskDialogProxy::source() const
{
    ViewFilter* filter = qobject_cast<ViewFilter*>( sourceModel() );
    Q_ASSERT( filter );
    return filter;
}

Task SelectTaskDialogProxy::taskForIndex( const QModelIndex& index ) const
{
    return source()->taskForIndex( mapToSource( index ) );
}

bool SelectTaskDialogProxy::taskIsActive( const Task& task ) const
{
    return source()->taskIsActive( task );
}

bool SelectTaskDialogProxy::taskHasChildren( const Task& task ) const
{
    return source()->taskHasChildren( task );
}

bool SelectTaskDialogProxy::taskIdExists( TaskId taskId ) const
{
    return source()->taskIdExists( taskId );
}

SelectTaskDialog::SelectTaskDialog( QWidget* parent )
    : QDialog( parent )
{
    m_ui.setupUi( this );
    m_ui.treeView->setModel( &m_proxy );
    // FIXME TEMP_REM
    // m_proxy.setSourceModel( MODEL.taskModel() );
    connect( m_ui.treeView->selectionModel(),
             SIGNAL( currentChanged( const QModelIndex&, const QModelIndex& ) ),
             SLOT( slotCurrentItemChanged( const QModelIndex&, const QModelIndex& ) ) );

    connect( m_ui.lineEditFilter, SIGNAL( textChanged( QString ) ),
             SLOT( slotFilterTextChanged( QString ) ) );

    m_ui.buttonClearFilter->setIcon( Data::clearFilterIcon() );
}

TaskId SelectTaskDialog::selectedTask() const
{
    return m_selectedTask;
}

void SelectTaskDialog::slotCurrentItemChanged( const QModelIndex& first,
                                               const QModelIndex& )
{
    Task task = m_proxy.taskForIndex( first );
    if ( task.isValid() ) {
        m_selectedTask = task.id();
    }
    m_ui.buttonBox->button( QDialogButtonBox::Ok )->setEnabled( m_selectedTask != 0 );
}

void SelectTaskDialog::slotFilterTextChanged( const QString& text )
{
    QString filtertext = text.simplified();
    filtertext.replace( ' ', '*' );
    m_proxy.setFilterWildcard( filtertext );

    m_ui.buttonClearFilter->setEnabled( ! text.isEmpty() );
}

#include "SelectTaskDialog.moc"

