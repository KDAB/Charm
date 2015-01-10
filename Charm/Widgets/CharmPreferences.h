/*
  CharmPreferences.h

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

#ifndef CHARMPREFERENCES_H
#define CHARMPREFERENCES_H

#include <QDialog>

#include "Core/Configuration.h"
#include "ui_CharmPreferences.h"

class CharmPreferences : public QDialog
{
    Q_OBJECT

public:
    explicit CharmPreferences( const Configuration& config,
                               QWidget* parent = nullptr );
    ~CharmPreferences();

    Configuration::DurationFormat durationFormat() const;
    bool detectIdling() const;
    bool warnUnuploadedTimesheets() const;
    bool requestEventComment() const;
    bool enableCommandInterface() const;

    Qt::ToolButtonStyle toolButtonStyle() const;

    Configuration::TimeTrackerFontSize timeTrackerFontSize() const;
private slots:
    void slotWarnUnuploadedChanged(bool);
    void slotResetPassword();

private:
    Ui::CharmPreferences m_ui;
};

#endif
