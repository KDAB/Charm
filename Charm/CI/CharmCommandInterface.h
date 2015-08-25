/*
  CharmCommandInterface.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2015-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Guillermo A. Amaral <gamaral@kdab.com>

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

#ifndef CHARM_CI_CHARMCOMMANDINTERFACE_H
#define CHARM_CI_CHARMCOMMANDINTERFACE_H

#include <QObject>

class CharmCommandServer;

class CharmCommandInterface : public QObject
{
    Q_OBJECT
public:
    explicit CharmCommandInterface(QObject* parent = nullptr);
    ~CharmCommandInterface();

    bool isStarted() const;
    void start();
    void stop();

public slots:
    void configurationChanged();

private:
    QList<CharmCommandServer *> m_servers;
};

#endif // CHARM_CI_CHARMCOMMANDINTERFACE_H
