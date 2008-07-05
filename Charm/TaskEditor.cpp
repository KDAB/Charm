/*
 * TaskEditor.cpp
 *
 *  Created on: Jul 4, 2008
 *      Author: mirko
 */
#include <QCheckBox>
#include <QMessageBox>

#include "Core/CharmConstants.h"
#include "Core/TaskTreeItem.h"
#include "Core/CharmDataModel.h"

#include "ViewHelpers.h"
#include "TaskEditor.h"
#include "Reports/CharmReport.h"
#include "ui_TaskEditor.h"
#include "SelectTaskDialog.h"

TaskEditor::TaskEditor( QWidget* parent )
	: QDialog( parent )
	, m_ui( new Ui::TaskEditor() )
{
	m_ui->setupUi( this );
	connect( m_ui->pushButtonParent, SIGNAL( clicked() ),
			 SLOT( slotSelectParent() ) );
	connect( m_ui->dateEditFrom, SIGNAL( dateChanged( QDate ) ),
			 SLOT( slotDateChanged( QDate ) ) );
	connect( m_ui->dateEditTo, SIGNAL( dateChanged( QDate ) ),
			 SLOT( slotDateChanged( QDate ) ) );
	connect( m_ui->checkBoxFrom, SIGNAL( clicked( bool ) ),
			 SLOT( slotCheckBoxChecked( bool ) ) );
	connect( m_ui->checkBoxUntil, SIGNAL( clicked( bool ) ),
			 SLOT( slotCheckBoxChecked( bool ) ) );
}

TaskEditor::~TaskEditor()
{
	delete m_ui; m_ui = 0;
}

void TaskEditor::setTask( const Task& task )
{
	Q_ASSERT( m_ui );
	m_task = task;
	const TaskTreeItem& taskTreeItem =
        MODEL.charmDataModel()->taskTreeItem( task.id() );
	m_ui->labelTaskName->setText( tasknameWithParents( taskTreeItem.task() ) );
	m_ui->lineEditName->setText( task.name() );
	if( task.parent() == 0 ) {
		m_ui->checkBoxTopLevel->setChecked( true );
	} else {
		m_ui->checkBoxTopLevel->setChecked( false );
	}
	QDate start = task.validFrom().date();
	if( start.isValid() ) {
		m_ui->dateEditFrom->setDate( start );
		m_ui->checkBoxFrom->setChecked( false );
	} else {
		m_ui->checkBoxFrom->setChecked( true );
		m_ui->dateEditFrom->setDate( QDate::currentDate() );
	}
	QDate end = task.validUntil().date();
	if( end.isValid() ) {
		m_ui->dateEditTo->setDate( end );
		m_ui->checkBoxUntil->setChecked( false );
	} else {
		m_ui->checkBoxUntil->setChecked( true );
		m_ui->dateEditTo->setDate( QDate::currentDate() );
	}
	checkInvariants();
}

Task TaskEditor::getTask() const
{
	m_task.setName( m_ui->lineEditName->text() );
	if( m_ui->checkBoxTopLevel->isChecked() ) {
		m_task.setParent( 0 );
	}
	if( m_ui->checkBoxFrom->isChecked() ) {
		m_task.setValidFrom( QDateTime() );
	} else {
		m_task.setValidFrom( m_ui->dateEditFrom->dateTime() );
	}
	if( m_ui->checkBoxUntil->isChecked() ) {
		m_task.setValidUntil( QDateTime() );
	} else {
		m_task.setValidUntil( m_ui->dateEditTo->dateTime() );
	}
	return m_task;
}

bool parentTreeIsTree( TaskId newParent, TaskId task )
{
	QList<TaskId> parents;
	TaskId parent = newParent;
	while( parent != 0 ) {
		parents << parent;
		const TaskTreeItem& parentItem =
				        MODEL.charmDataModel()->taskTreeItem( parent );
		parent = parentItem.task().parent();
	}
	return ! parents.contains( task );
}

void TaskEditor::slotSelectParent()
{
    SelectTaskDialog dialog( this );
    Q_FOREVER {
		if ( dialog.exec() ) {
			TaskId newParentId = dialog.selectedTask();
			if( parentTreeIsTree( newParentId, m_task.id() ) ) {
				m_task.setParent( dialog.selectedTask() );
				break;
			}
			QString name = m_task.name();
			const TaskTreeItem& parentItem =
		        MODEL.charmDataModel()->taskTreeItem( dialog.selectedTask() );
			QString parent = parentItem.task().name();
			QMessageBox::information( this, tr( "Please choose another task" ),
                    tr( "The task \"%1\" cannot be selected as the parent task for \"%2\","
                    	" because they are the same, or \"%3\" is a direct or indirect subtask of \"%4\".")
                    	.arg( parent ).arg( name ).arg( parent ).arg( name ) );
		} else {
			break;
		}
    }
}

void TaskEditor::slotDateChanged( const QDate& )
{
	checkInvariants();
}

void TaskEditor::slotCheckBoxChecked( bool )
{
	checkInvariants();
}

void TaskEditor::checkInvariants()
{
	bool acceptable;
	// the start and end dates are acceptable if
	// * both start and end date are deactivated,
	// * if only one date is set, or
	// * if none are set, and the start date is before the end date
	if( m_ui->checkBoxFrom->isChecked() && m_ui->checkBoxUntil->isChecked() ) {
		acceptable = true;
	} else if ( m_ui->checkBoxFrom->isChecked() || m_ui->checkBoxUntil->isChecked() ) {
		acceptable = true;
	} else if ( m_ui->dateEditFrom->dateTime() < m_ui->dateEditTo->dateTime() ) {
		acceptable = true;
	} else {
		acceptable = false;
	}

	m_ui->buttonBox->button( QDialogButtonBox::Ok )
		->setEnabled( acceptable );
}
#include "TaskEditor.moc"
