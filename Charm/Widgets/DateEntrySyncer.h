/*
  DateEntrySyncer.h

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

#ifndef DATEENTRYSYNCER_H
#define DATEENTRYSYNCER_H

#include <QObject>

class QSpinBox;
class QDateEdit;

class DateEntrySyncer : public QObject
{
    Q_OBJECT
public:
    DateEntrySyncer( QSpinBox* weekNumberSB, QSpinBox* yearSB, QDateEdit* dateedit, int weekDay=1, QObject* parent=nullptr );

private Q_SLOTS:
    void dateSelectionChanged();

private:
    QSpinBox* m_week;
    QSpinBox* m_year;
    QDateEdit* m_date;
    int m_weekDay;
};

#endif //DATEENTRYSYNCER_H
