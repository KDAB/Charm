/*
  TasksViewDelegate.cpp

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

#include "TasksViewDelegate.h"
#include "TaskModelAdapter.h"
#include "ViewHelpers.h"

#include <QEvent>
#include <QMouseEvent>
#include <QPainter>

TasksViewDelegate::TasksViewDelegate( QObject* parent )
    : QItemDelegate( parent )
    , m_editing( false )
{
    connect( this, SIGNAL(closeEditor(QWidget*,QAbstractItemDelegate::EndEditHint)),
             SLOT(slotCloseEditor(QWidget*,QAbstractItemDelegate::EndEditHint)) );

}

QWidget* TasksViewDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    QWidget* result = QItemDelegate::createEditor( parent, option, index );
    m_editing = true;
    emit editingStateChanged();
    return result;
}

bool TasksViewDelegate::isEditing() const
{
    return m_editing;
}

void TasksViewDelegate::slotCloseEditor( QWidget*, QAbstractItemDelegate::EndEditHint )
{
    m_editing = false;
    emit editingStateChanged();
}

void TasksViewDelegate::paint( QPainter *painter,
                               const QStyleOptionViewItem &option,
                               const QModelIndex &index ) const
{
    painter->save();

    // Nasty QTreeView clips the painting to the editor geometry!
    // We don't want that....
    painter->setClipRect( option.rect );

    drawBackground( painter, option, index );

    const QVariant checkStateVariant = index.data(Qt::CheckStateRole);
    const Qt::CheckState checkState = static_cast<Qt::CheckState>(checkStateVariant.toInt());

    Layout layout = doLayout( option, index );

    const QRect textRect(option.rect.left(),
                         option.rect.top(),
                         option.rect.width() - layout.cbRect.width(),
                         layout.height);

    // Prepare QStyleOptionViewItem with the wanted alignments
    QStyleOptionViewItem modifiedOption = option;
    modifiedOption.displayAlignment = Qt::AlignLeft | Qt::AlignVCenter;
    modifiedOption.decorationAlignment = Qt::AlignLeft | Qt::AlignVCenter;

    // Draw text (task id+name)
    const QString taskName = index.data(Qt::DisplayRole).toString();
    QString elidedTask = Charm::elidedTaskName( taskName, painter->font(),
                                                textRect.width() );
    drawDisplay(painter, modifiedOption, textRect, elidedTask);

    // Draw checkbox
    drawCheck(painter, option, layout.cbRect, checkState);
    painter->restore();
}

QSize TasksViewDelegate::sizeHint( const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const
{
    Layout layout = doLayout( option, index );
    return QSize(option.rect.width(), layout.height);
}

bool TasksViewDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
                                     const QStyleOptionViewItem &option, const QModelIndex &index)
{
    Q_ASSERT(event);
    Q_ASSERT(model);

    // make sure that the item is checkable
    Qt::ItemFlags flags = model->flags(index);
    if (!(flags & Qt::ItemIsUserCheckable) || !(option.state & QStyle::State_Enabled)
        || !(flags & Qt::ItemIsEnabled))
        return false;

    // make sure that we have a check state
    QVariant value = index.data(Qt::CheckStateRole);
    if (!value.isValid())
        return false;

    // make sure that we have the right event type
    if ((event->type() == QEvent::MouseButtonRelease)
        || (event->type() == QEvent::MouseButtonDblClick)) {
        const QRect checkRect = checkBoxRect(option, Qt::Checked);
        QMouseEvent *me = static_cast<QMouseEvent*>(event);
        if (me->button() != Qt::LeftButton || !checkRect.contains(me->pos()))
            return false;

        // eat the double click events inside the check rect
        if (event->type() == QEvent::MouseButtonDblClick)
            return true;

    } else if (event->type() == QEvent::KeyPress) {
        if (static_cast<QKeyEvent*>(event)->key() != Qt::Key_Space
         && static_cast<QKeyEvent*>(event)->key() != Qt::Key_Select)
            return false;
    } else {
        return false;
    }

    Qt::CheckState state = (static_cast<Qt::CheckState>(value.toInt()) == Qt::Checked
                            ? Qt::Unchecked : Qt::Checked);
    return model->setData(index, state, Qt::CheckStateRole);
}

QRect TasksViewDelegate::checkBoxRect( const QStyleOptionViewItem &option,
                                       const QVariant &variant ) const
{
    const QRect bounding = option.rect; // TODO adjust if recording

#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
    const QRect cbRect = check(option, bounding, variant);
#else
    const QRect cbRect = doCheck(option, bounding, variant);
#endif

    // Position checkbox on the right, and vertically aligned
    return QStyle::alignedRect(option.direction, Qt::AlignRight | Qt::AlignVCenter,
                               cbRect.size(), bounding);
}

void TasksViewDelegate::updateEditorGeometry( QWidget * editor,
                                              const QStyleOptionViewItem & option,
                                              const QModelIndex & index ) const
{
    // TODO use doLayout
    const QRect cbRect = checkBoxRect(option, Qt::Checked);
    int firstLineHeight = qMax(cbRect.height(), option.fontMetrics.height());
    const QVariant decorationVariant = index.data(Qt::DecorationRole);
    const QPixmap decorationPixmap = decoration(option, decorationVariant);
    const QString runningTime = index.data(TasksViewRole_RunningTime).toString();
    const int left = decorationPixmap.width() + option.fontMetrics.width(runningTime);
    QRect r = option.rect.translated( left + 5, 0 );
    r.setRight( cbRect.left() );
    r.setTop( r.top() + firstLineHeight );
    editor->setGeometry( r );
}

void TasksViewDelegate::setEditorData( QWidget * editor, const QModelIndex & index ) const
{
    // Do not reset the comment lineedit to empty every time TaskModelAdapter emits
    // dataChanged (because of the running time being updated).
    if ( m_editing )
        return;
    QItemDelegate::setEditorData( editor, index );
}

TasksViewDelegate::Layout TasksViewDelegate::doLayout( const QStyleOptionViewItem& option,
                                                       const QModelIndex& index ) const
{
    Layout layout;
    // Find size of checkbox
    const QVariant checkStateVariant = index.data(Qt::CheckStateRole);
    layout.cbRect = checkBoxRect(option, checkStateVariant);
    layout.height = qMax(layout.cbRect.height(), option.fontMetrics.height());

    return layout;
}

#include "moc_TasksViewDelegate.cpp"
