/*
  BackendIntegrationTests.cpp

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

#include "BackendIntegrationTests.h"

#include "Core/CharmConstants.h"
#include "Core/Controller.h"
#include "Core/TaskTreeItem.h"
#include "Core/Configuration.h"
#include "Core/CharmDataModel.h"
#include "Core/StorageInterface.h"

#include <QDir>
#include <QFileInfo>
#include <QtDebug>
#include <QtTest/QtTest>

BackendIntegrationTests::BackendIntegrationTests()
    : TestApplication("./BackendIntegrationTestDatabase.db")
{
}

void BackendIntegrationTests::initTestCase()
{
    initialize();
}

void BackendIntegrationTests::initialValuesTest()
{
    // storage:
    QVERIFY( controller()->storage()->getAllTasks().isEmpty() );
    QVERIFY( controller()->storage()->getAllEvents().isEmpty() );
    QVERIFY( controller()->storage()->getUser( testUserId() ).isValid() );
    QVERIFY( controller()->storage()->getInstallation( testInstallationId() ).isValid() );
    // model:
    QVERIFY( model()->taskTreeItem( 0 ).childCount() == 0 );
}

void BackendIntegrationTests::simpleCreateModifyDeleteTaskTest()
{
    Task task1( 1000, "Task 1" );
    Task task1b( task1 );
    task1b.setName( "Task 1, modified" );
    // add:
    controller()->addTask( task1 );
    QVERIFY( controller()->storage()->getAllTasks().size() == 1 );
    QVERIFY( controller()->storage()->getAllTasks().first() == task1 );
    QVERIFY( model()->taskTreeItem( task1.id() ).task() == task1 );
    // modify:
    controller()->modifyTask( task1b );
    QVERIFY( controller()->storage()->getAllTasks().size() == 1 );
    QVERIFY( controller()->storage()->getAllTasks().first() == task1b );
    QVERIFY( model()->taskTreeItem( task1.id() ).task() == task1b );
    // delete:
    controller()->deleteTask( task1 );
    QVERIFY( controller()->storage()->getAllTasks().size() == 0 );
    QVERIFY( model()->taskTreeItem( 0 ).childCount() == 0 );
}

void BackendIntegrationTests::biggerCreateModifyDeleteTaskTest()
{
    const TaskList& tasks = referenceTasks();
    // make sure everything is cleaned up:
    QVERIFY( controller()->storage()->getAllTasks().isEmpty() );
    QVERIFY( model()->taskTreeItem( 0 ).childCount() == 0 );

    // add one task after the other, and compare the lists in storage
    // and in the model:
    TaskList currentTasks;
    for ( int i = 0; i < tasks.size(); ++ i )
    {
        currentTasks.append( tasks[i] );
        controller()->addTask( tasks[i] );
        QVERIFY( contentsEqual( controller()->storage()->getAllTasks(), currentTasks ) );
        QVERIFY( contentsEqual( model()->getAllTasks(), currentTasks ) );
    }
    // modify the tasks:
    for ( int i = 0; i < currentTasks.size(); ++i )
    {
        currentTasks[i].setName( currentTasks[i].name() + " - modified" );
        controller()->modifyTask( currentTasks[i] );
        QVERIFY( contentsEqual( controller()->storage()->getAllTasks(), currentTasks ) );
        QVERIFY( contentsEqual( model()->getAllTasks(), currentTasks ) );
    }
    // delete the tasks (in reverse, because they depend on each
    // other):
    for ( int i = currentTasks.size(); i > 0; --i )
    {
        controller()->deleteTask( currentTasks[i-1] );
        currentTasks.removeAt( i-1 );
        QVERIFY( contentsEqual( controller()->storage()->getAllTasks(), currentTasks ) );
        QVERIFY( contentsEqual( model()->getAllTasks(), currentTasks ) );
    }
    // all gone?
    QVERIFY( controller()->storage()->getAllTasks().isEmpty() );
    QVERIFY( model()->taskTreeItem( 0 ).childCount() == 0 );
}

void BackendIntegrationTests::cleanupTestCase ()
{
    destroy();
}

const TaskList& BackendIntegrationTests::referenceTasks()
{
    static TaskList Tasks;
    if ( Tasks.isEmpty() ) {
        Task task1( 1000, "Task 1" );
        Task task1_1( 1001, "Task 1-1", task1.id() );
        Task task1_2( 1002, "Task 1-2", task1.id() );
        Task task1_3( 1003, "Task 1-3", task1.id() );
        Task task2( 2000, "Task 2" );
        Task task2_1( 2100, "Task 2-1", task2.id() );
        Task task2_1_1( 2110, "Task 2-1-1", task2_1.id() );
        Task task2_1_2( 2120, "Task 2-1-2", task2_1.id() );
        Task task2_2( 2200, "Task 2-2", task2.id() );
        Task task2_2_1( 2210, "Task 2-2-1", task2_2.id() );
        Task task2_2_2( 2220, "Task 2-2-2", task2_2.id() );
        Tasks << task1 << task1_1 << task1_2 << task1_3
              << task2 << task2_1 << task2_1_1 << task2_1_2
              << task2_2 << task2_2_1 << task2_2_2;
    }
    return Tasks;
}

bool BackendIntegrationTests::contentsEqual( const TaskList& listref1,
                                             const TaskList& listref2 )
{
    TaskList list1( listref1 );
    TaskList list2( listref2 );

    for ( int i = 0; i < list1.size(); ++i )
    {
        for ( int j = 0; j < list2.size(); ++j )
        {
            if ( list2[j] == list1[i] ) {
                list2.removeAt( j );
            }
        }
    }

    return list2.isEmpty();
}

QTEST_MAIN( BackendIntegrationTests )

#include "moc_BackendIntegrationTests.cpp"
