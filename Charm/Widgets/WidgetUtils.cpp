/*
  WidgetUtils.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2016-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Hannah von Reth <hannah.vonreth@kdab.com>

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

#include "WidgetUtils.h"

#include <Core/CharmConstants.h>

#include <QSettings>
#include <QToolButton>
#include <QWidget>

void WidgetUtils::restoreGeometry(QWidget *widget, const QString &metaKey)
{
    QSettings settings;
    settings.beginGroup(QStringLiteral("WindowStates"));
    if (settings.contains(metaKey))
        widget->restoreGeometry(settings.value(metaKey).toByteArray());
}

void WidgetUtils::saveGeometry(QWidget *widget, const QString &metaKey)
{
    QSettings settings;
    settings.beginGroup(QStringLiteral("WindowStates"));
    settings.setValue(metaKey, widget->saveGeometry());
}

void WidgetUtils::updateToolButtonStyle(QWidget *widget)
{
    const QList<QToolButton *> buttons = widget->findChildren<QToolButton *>();
    Q_FOREACH (auto button, buttons)
        button->setToolButtonStyle(CONFIGURATION.toolButtonStyle);
}
