/*
  TasksViewDelegate.h

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

#ifndef TASKSVIEWDELEGATE_H
#define TASKSVIEWDELEGATE_H

#include <QItemDelegate>

/**
 * Delegate for the tasks view and for the "select task" dialog.
 */
class TasksViewDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    explicit TasksViewDelegate( QObject* parent = nullptr );

    void paint( QPainter *painter,
                const QStyleOptionViewItem &option,
                const QModelIndex &index ) const override;
    QSize sizeHint( const QStyleOptionViewItem &option,
                    const QModelIndex &index ) const override;

    QWidget* createEditor( QWidget* parent,
                           const QStyleOptionViewItem& option,
                           const QModelIndex& index ) const override;
    bool editorEvent( QEvent *event, QAbstractItemModel *model,
                      const QStyleOptionViewItem &option,
                      const QModelIndex &index ) override;
    void updateEditorGeometry( QWidget * editor,
                               const QStyleOptionViewItem & option,
                               const QModelIndex & index ) const override;
    void setEditorData( QWidget * editor, const QModelIndex & index ) const override;

    bool isEditing() const;

signals:
    void editingStateChanged() const;

private slots:
    void slotCloseEditor( QWidget* editor, QAbstractItemDelegate::EndEditHint );

private:
    QRect checkBoxRect( const QStyleOptionViewItem &option,
                        const QVariant &variant ) const;
    struct Layout {
        int height;
        QRect cbRect;
    };
    Layout doLayout( const QStyleOptionViewItem& option,
                     const QModelIndex& index ) const;
    mutable bool m_editing;
};
#endif
