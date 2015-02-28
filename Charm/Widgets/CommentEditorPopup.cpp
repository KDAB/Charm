/*
  CommentEditorPopup.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2015-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mathias Hasselmann <mathias.hasselmann@kdab.com>

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

#include "CommentEditorPopup.h"
#include "ui_CommentEditorPopup.h"

#include <QMessageBox>
#include <QPointer>
#include <QPushButton>
#include <QTextEdit>

#include "ViewHelpers.h"

CommentEditorPopup::CommentEditorPopup( QWidget *parent )
    : QDialog( parent )
    , ui( new Ui::CommentEditorPopup )
    , m_id()
{
    ui->setupUi( this );
    ui->buttonBox->button( QDialogButtonBox::Ok )->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_Return ) );
    ui->textEdit->setFocus( Qt::TabFocusReason );
}

CommentEditorPopup::~CommentEditorPopup()
{
    delete ui;
}

void CommentEditorPopup::loadEvent(EventId id)
{
    Event event = DATAMODEL->eventForId( id );

    if ( !event.isValid() ) {
        m_id = EventId();
        return;
    }

    m_id = id;
    ui->textEdit->setPlainText( event.comment() );
}

void CommentEditorPopup::accept()
{
    const QString t = ui->textEdit->toPlainText();
    Event event = DATAMODEL->eventForId( m_id );
    if ( event.isValid() ) {
        event.setComment( t );
        DATAMODEL->modifyEvent( event );
    } else { // event already gone? should never happen, but you never know
        QPointer<QObject> that( this );
        QMessageBox::critical( this, tr("Error"), tr("Could not save the comment, the edited event was deleted in the meantime."), QMessageBox::Ok );
        if ( !that ) // in case the popup was deleted while the msg box was open
            return;
    }
    QDialog::accept();
}
