/*
  SqLiteStorageTests.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2007-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>

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

#ifndef SQLITESTORAGETEST_H
#define SQLITESTORAGETEST_H

#include <QObject>

#include "Core/Configuration.h"
#include "Core/StorageInterface.h"

class SqLiteStorageTests : public QObject
{
    Q_OBJECT
public:
    SqLiteStorageTests();
    ~SqLiteStorageTests();

private:
    StorageInterface* m_storage;
    Configuration m_configuration;
    QString m_localPath;

private slots:
    void initTestCase ();

    void connectAndCreateDatabaseTest();

    void makeModifyDeleteInstallationTest();

    void makeModifyDeleteUserTest();

    void makeModifyDeleteTasksTest();

    void makeModifyDeleteEventsTest();

    void addDeleteSubscriptionsTest();

    void setGetMetaDataTest();

    void deleteTaskWithEventsTest();

    void cleanupTestCase();
};

#endif
