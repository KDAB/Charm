/*
  IdleCorrectionDialog.cpp

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

#include "IdleCorrectionDialog.h"
#include "ui_IdleCorrectionDialog.h"

#include <Core/CharmConstants.h>

#include <QTimer>

IdleCorrectionDialog::IdleCorrectionDialog(const IdleDetector::IdlePeriod &idlePeriod,
                                           QWidget *parent)
    : QDialog(parent)
    , m_ui(new Ui::IdleCorrectionDialog)
    , m_start(idlePeriod.first)
{
    m_ui->setupUi(this);

    updateDuration();

    auto timer = new QTimer(this);
    timer->setInterval(60000);

    connect(timer, &QTimer::timeout, this, &IdleCorrectionDialog::updateDuration);
    timer->start();
}

IdleCorrectionDialog::~IdleCorrectionDialog()
{
}

IdleCorrectionDialog::Result IdleCorrectionDialog::result() const
{
    if (m_ui->ignore->isChecked()) {
        return Idle_Ignore;
    } else if (m_ui->endEvent->isChecked()) {
        return Idle_EndEvent;
    } else if (m_ui->restartEvent->isChecked()) {
        return Idle_RestartEvent;
    } else {
        Q_ASSERT(false);   // unhandled whatever?
    }

    return Idle_NoResult;
}

void IdleCorrectionDialog::updateDuration()
{
    const auto secs = m_start.secsTo(QDateTime::currentDateTime());
    m_ui->idleLabel->setText(
        tr(
            "Charm detected that the computer became idle for %1 hours, while an event was in progress.")
        .arg(hoursAndMinutes(secs)));
}
