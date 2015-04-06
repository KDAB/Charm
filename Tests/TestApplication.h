/*
  TestApplication.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>
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

#ifndef TESTAPPLICATION_H
#define TESTAPPLICATION_H

#include <QObject>

class Controller;
class CharmDataModel;
class Configuration;

class TestApplication : public QObject
{
    Q_OBJECT
public:
    explicit TestApplication(const QString &databasePath, QObject *parent = nullptr);

    void initialize();
    void destroy();
    int testUserId() const;
    int testInstallationId() const;

protected:
    Controller* controller() const;
    CharmDataModel* model() const;
    Configuration* configuration() const;
    QString databasePath() const;

private:
    Controller* m_controller;
    CharmDataModel* m_model;
    Configuration* m_configuration;
    QString m_localPath;
};

#endif // TESTAPPLICATION_H
