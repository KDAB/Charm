/*
 * TaskEditor.cpp
 *
 *  Created on: Jul 4, 2008
 *      Author: mirko
 */
#include <QCheckBox>

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

void TaskEditor::slotSelectParent()
{
    SelectTaskDialog dialog( this );
    if ( dialog.exec() ) {
    	m_task.setParent( dialog.selectedTask() );
    }
}

#include "TaskEditor.moc"
