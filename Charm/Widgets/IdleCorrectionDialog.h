/*
  IdleCorrectionDialog.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2014-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

#ifndef IDLECORRECTIONDIALOG_H
#define IDLECORRECTIONDIALOG_H

#include "Idle/IdleDetector.h"

#include <QDialog>
#include <QScopedPointer>

namespace Ui {
class IdleCorrectionDialog;
}

class IdleCorrectionDialog : public QDialog
{
    Q_OBJECT

public:
    enum Result {
        Idle_NoResult,
        Idle_Ignore,
        Idle_EndEvent,
        Idle_RestartEvent
    };

    explicit IdleCorrectionDialog(const IdleDetector::IdlePeriod &idlePeriod,
                                  QWidget *parent = nullptr);
    ~IdleCorrectionDialog() override;

    Result result() const;

private:
    void updateDuration();

    QScopedPointer<Ui::IdleCorrectionDialog> m_ui;
    QDateTime m_start;
};

#endif
