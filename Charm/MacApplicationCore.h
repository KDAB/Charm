/*
  MacApplicationCore.h

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

#ifndef MACAPPLICATIONCORE_H
#define MACAPPLICATIONCORE_H

#include "ApplicationCore.h"

class MacApplicationCore : public ApplicationCore
{
    Q_OBJECT
public:
    explicit MacApplicationCore( QObject* parent = nullptr );
    ~MacApplicationCore();
    // This method to be public due to lack of friend classes in Objective-C and
    // the lack inheritance of Objective-C classes from C++ ones.
    void dockIconClickEvent();

private slots:
    void handleStateChange( State state ) const;

private:
    static QList< QShortcut* > shortcuts( QWidget* parent );
    static QList< QShortcut* > activeShortcuts( const QKeySequence& seq, bool autorep, QWidget* parent = nullptr);

    QMenu m_dockMenu;

    class Private;
    Private* m_private;
};

#endif
