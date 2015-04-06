/*
  TestApplication.cpp

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

#include "TestApplication.h"

#include "Core/Configuration.h"
#include "Core/CharmConstants.h"
#include "Core/Controller.h"
#include "Core/CharmDataModel.h"

#include <QString>
#include <QFileInfo>
#include <QtDebug>
#include <QDir>
#include <QtTest/QtTest>

const int UserId = 1;
const int InstallationId = 1;

TestApplication::TestApplication(const QString &databasePath, QObject *parent)
    : QObject(parent)
    , m_controller( nullptr )
    , m_model( nullptr )
    , m_configuration( &Configuration::instance() )
    , m_localPath( databasePath )
{
}

Controller* TestApplication::controller() const
{
    return m_controller;
}

CharmDataModel* TestApplication::model() const
{
    return m_model;
}

Configuration* TestApplication::configuration() const
{
    return m_configuration;
}

QString TestApplication::databasePath() const
{
    return m_localPath;
}

void TestApplication::initialize()
{
    QFileInfo file( m_localPath );
    if ( file.exists() ) {
        qDebug() << "test database file exists, deleting";
        QDir dir( file.absoluteDir() );
        QVERIFY( dir.remove( file.fileName() ) );
    }

    // well, here it gets a bit more challenging - this is not for
    // sissies:
    // - make a controller
    // - make it create a local storage backend
    // - make a data model and connect it to the controller
    // - stimulate the controller and see if the right content ends up
    //   in the database and the model
    // -----
    // ... make the controller:
    m_configuration->installationId = InstallationId;
    m_configuration->user.setId( UserId );
    m_configuration->localStorageType = CHARM_SQLITE_BACKEND_DESCRIPTOR;
    m_configuration->localStorageDatabase = m_localPath;
    m_configuration->newDatabase = true;
    m_controller = new Controller;
    // ... initialize the backend:
    QVERIFY( m_controller->initializeBackEnd( CHARM_SQLITE_BACKEND_DESCRIPTOR ) );
    QVERIFY( m_controller->connectToBackend() );
    // ... make the data model:
    m_model = new CharmDataModel;
    // ... connect model and controller:
    connectControllerAndModel( m_controller, m_model );
    QVERIFY( m_controller->storage() != nullptr );
}

void TestApplication::destroy()
{
    QVERIFY( controller()->disconnectFromBackend() );
    delete m_model; m_model = nullptr;
    delete m_controller; m_controller = nullptr;
    if ( QDir::home().exists( databasePath() ) ) {
        const bool result = QDir::home().remove( databasePath() );
        QVERIFY( result );
    }
}

int TestApplication::testUserId() const
{
    return 1;
}

int TestApplication::testInstallationId() const
{
    return 1;
}

#include "moc_TestApplication.cpp"
