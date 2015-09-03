/*
  SelectTaskDialog.h

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

#ifndef SELECTTASKDIALOG_H
#define SELECTTASKDIALOG_H

#include <QDialog>
#include <QHash>
#include <QScopedPointer>

#include "ViewFilter.h"

class ViewFilter;
class CharmDataModel;

namespace Ui {
    class SelectTaskDialog;
}

class SelectTaskDialogProxy : public ViewFilter
{
    Q_OBJECT

public:
    explicit SelectTaskDialogProxy( CharmDataModel*, QObject* parent = nullptr );

    Qt::ItemFlags flags( const QModelIndex & index ) const override;
    QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const override;

protected:
    bool filterAcceptsColumn( int column, const QModelIndex& parent ) const override;
};

class SelectTaskDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectTaskDialog( QWidget* parent=nullptr );
    ~SelectTaskDialog();

    TaskId selectedTask() const;
    void setNonTrackableSelectable();

signals:
    void saveConfiguration();

protected:
    void showEvent( QShowEvent * event ) override;
    void hideEvent( QHideEvent* event ) override;

private slots:
    void slotCurrentItemChanged( const QModelIndex&, const QModelIndex& );
    void slotDoubleClicked ( const QModelIndex & );
    void slotFilterTextChanged( const QString& );
    void slotAccepted();
    void slotPrefilteringChanged();
    void slotResetState();

private:
    bool isValidAndTrackable( const QModelIndex& index ) const;

private:
    QScopedPointer<Ui::SelectTaskDialog> m_ui;
    TaskId m_selectedTask;
    SelectTaskDialogProxy m_proxy;
    QHash<TaskId,bool> m_expansionStates;
    bool m_nonTrackableSelectable;
};

#endif
