/*
  SelectTaskDialog.cpp

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

#include "SelectTaskDialog.h"
#include "ExpandStatesHelper.h"
#include "GUIState.h"
#include "ViewHelpers.h"

#include <QPushButton>
#include <QSettings>

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
    , m_selectedTask( 0 )
    , m_proxy( MODEL.charmDataModel() )
    , m_nonTrackableSelectable( false )
{
    m_ui->setupUi( this );
    m_ui->treeView->setModel( &m_proxy );
    m_ui->treeView->header()->hide();
    m_ui->buttonBox->button( QDialogButtonBox::Cancel )->setEnabled( true );
    m_ui->buttonBox->button( QDialogButtonBox::Ok )->setEnabled( false );
    connect( m_ui->treeView->selectionModel(),
             SIGNAL(currentChanged(QModelIndex,QModelIndex)),
             SLOT(slotCurrentItemChanged(QModelIndex,QModelIndex)) );
    connect( m_ui->treeView,
             SIGNAL(doubleClicked(QModelIndex)),
             SLOT(slotDoubleClicked(QModelIndex)) );

    connect( m_ui->filter, SIGNAL(textChanged(QString)),
             SLOT(slotFilterTextChanged(QString)) );
    connect( m_ui->showExpired, SIGNAL(toggled(bool)),
             SLOT(slotPrefilteringChanged()) );
    connect( m_ui->showSelected, SIGNAL(toggled(bool)),
             SLOT(slotPrefilteringChanged()) );
    connect( this, SIGNAL(accepted()),
             SLOT(slotAccepted()) );
    connect( MODEL.charmDataModel(), SIGNAL(resetGUIState()),
             SLOT(slotResetState()) );

    QSettings settings;
    settings.beginGroup( staticMetaObject.className() );
    if ( settings.contains( MetaKey_MainWindowGeometry ) ) {
        resize( settings.value( MetaKey_MainWindowGeometry ).toSize() );
    }
    // initialize prefiltering
    slotPrefilteringChanged();
    m_ui->filter->setFocus();
}

SelectTaskDialog::~SelectTaskDialog()
{
}

void SelectTaskDialog::slotResetState()
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

    m_ui->showExpired->setChecked( state.showExpired() );
    m_ui->showSelected->setChecked( state.showCurrents() );
}

void SelectTaskDialog::showEvent ( QShowEvent * event )
{
    slotResetState();
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
    const Task task = m_proxy.taskForIndex( first );
    if ( isValidAndTrackable( first ) ) {
        m_selectedTask = task.id();
        m_ui->taskStatusLB->clear();
    } else {
        m_selectedTask = 0;
        const bool expired = !task.isCurrentlyValid();
        const bool trackable = task.trackable();
        const bool notTrackableAndExpired = ( !trackable && expired );
        const QString expirationDate = QLocale::system().toString( task.validUntil(), QLocale::ShortFormat );
        const QString info = notTrackableAndExpired ? tr( "The selected task is not trackable and expired since %1" ).arg( expirationDate ) :
                                                      expired ? tr( "The selected task is expired since %1" ).arg( expirationDate ) :
                                                                tr( "The selected task is not trackable" );
        m_ui->taskStatusLB->setText( info );
    }

    m_ui->buttonBox->button( QDialogButtonBox::Ok )->setEnabled( m_selectedTask != 0 );
}


bool SelectTaskDialog::isValidAndTrackable( const QModelIndex& index ) const
{
    if ( !index.isValid() )
        return false;
    const Task task = m_proxy.taskForIndex( index );

    const bool taskValid = task.isValid() && task.isCurrentlyValid();

    if ( m_nonTrackableSelectable ) {
        return taskValid;
    }
    return taskValid && task.trackable();
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
    if ( ApplicationCore::instance().state() == Connected ||
        ApplicationCore::instance().state() == Disconnecting ) {
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
        state.setShowExpired( m_ui->showExpired->isChecked() );
        state.setShowCurrents( m_ui->showSelected->isChecked() );
        settings.beginGroup( staticMetaObject.className() );
        state.saveTo( settings );
    }
}

void SelectTaskDialog::slotPrefilteringChanged()
{
    // find out about the selected mode:
    Configuration::TaskPrefilteringMode mode;
    const bool showCurrentOnly = ! m_ui->showExpired->isChecked();
    const bool showSubscribedOnly = m_ui->showSelected->isChecked();
    if (  showCurrentOnly && showSubscribedOnly ) {
        mode = Configuration::TaskPrefilter_SubscribedAndCurrentOnly;
    } else if ( showCurrentOnly && ! showSubscribedOnly ) {
        mode = Configuration::TaskPrefilter_CurrentOnly;
    } else if ( ! showCurrentOnly && showSubscribedOnly ) {
        mode = Configuration::TaskPrefilter_SubscribedOnly;
    } else {
        mode = Configuration::TaskPrefilter_ShowAll;
    }

    CONFIGURATION.taskPrefilteringMode = mode;
    Charm::saveExpandStates( m_ui->treeView, &m_expansionStates );
    m_proxy.prefilteringModeChanged();
    Charm::restoreExpandStates( m_ui->treeView, &m_expansionStates );
    emit saveConfiguration();
}

void SelectTaskDialog::setNonTrackableSelectable()
{
    m_nonTrackableSelectable = true;
}

#include "moc_SelectTaskDialog.cpp"
