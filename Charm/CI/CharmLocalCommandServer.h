/*
  CharmLocalCommandServer.h

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

#ifndef CHARM_CI_CHARMLOCALCOMMANDSERVER_H
#define CHARM_CI_CHARMLOCALCOMMANDSERVER_H

#include "CharmCommandServer.h"

class QLocalServer;

class CharmLocalCommandServer : public CharmCommandServer
{
    Q_OBJECT
public:
    explicit CharmLocalCommandServer(QObject* parent = nullptr);
    ~CharmLocalCommandServer();

    bool listen() override;
    void close() override;

private slots:
    void onNewConnection();

private:
    QLocalServer* m_server;
};

#endif // CHARM_CI_CHARMLOCALSERVER_H
