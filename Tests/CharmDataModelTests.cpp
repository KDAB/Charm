/*
  CharmDataModelTests.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2007-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

#include "CharmDataModelTests.h"

#include "Core/Task.h"
#include "Core/TaskTreeItem.h"
#include "Core/CharmDataModel.h"

#include <QtDebug>
#include <QtTest/QtTest>

CharmDataModelTests::CharmDataModelTests()
    : QObject()
    , m_referenceModel( nullptr )
{
}

void CharmDataModelTests::initTestCase ()
{
    // set up a model that the other tests can clone to use:
    m_referenceModel = new CharmDataModel;
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
    TaskList tasks;
    tasks << task1 << task1_1 << task1_2 << task1_3
          << task2 << task2_1 << task2_1_1 << task2_1_2
          << task2_2 << task2_2_1 << task2_2_2;
    // test setAllTasks, only slightly, more detailed tests follow
    m_referenceModel->setAllTasks( tasks );
    QVERIFY( m_referenceModel->taskTreeItem( task2_2.id() ).childCount() == 2 );
    QVERIFY( m_referenceModel->taskTreeItem( task2.id() ).childCount() == 2 );
}


void CharmDataModelTests::createAndDestroyTest()
{
    CharmDataModel model;
}

void CharmDataModelTests::addAndRemoveTasksTest()
{   // set up a structure of tasks:
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

    // set up a data model, and add all the tasks to it, step by step:
    CharmDataModel model;
    QVERIFY( model.taskTreeItem( 0 ).childCount() == 0 );
    model.addTask( task1 );
    QVERIFY( model.taskTreeItem( 0 ).childCount() == 1 );
    QVERIFY( model.taskTreeItem( task1.id() ).childCount() == 0 );
    model.addTask( task1_1 );
    QVERIFY( model.taskTreeItem( task1.id() ).childCount() == 1 );
    model.addTask( task1_2 );
    QVERIFY( model.taskTreeItem( task1.id() ).childCount() == 2 );
    model.addTask( task1_3 );
    QVERIFY( model.taskTreeItem( 0 ).childCount() == 1 );
    QVERIFY( model.taskTreeItem( task1.id() ).childCount() == 3 );
    model.addTask( task2 );
    QVERIFY( model.taskTreeItem( 0 ).childCount() == 2 );
    model.addTask( task2_1 );
    QVERIFY( model.taskTreeItem( task2.id() ).childCount() == 1 );
    model.addTask( task2_2 );
    QVERIFY( model.taskTreeItem( task2.id() ).childCount() == 2 );
    QVERIFY( model.taskTreeItem( task2_1.id() ).childCount() == 0 );
    model.addTask( task2_1_1 );
    QVERIFY( model.taskTreeItem( task2_1.id() ).childCount() == 1 );
    model.addTask( task2_1_2 );
    QVERIFY( model.taskTreeItem( task2_1.id() ).childCount() == 2 );
    QVERIFY( model.taskTreeItem( task2_2.id() ).childCount() == 0 );
    model.addTask( task2_2_1 );
    QVERIFY( model.taskTreeItem( task2_2.id() ).childCount() == 1 );
    model.addTask( task2_2_2 );
    QVERIFY( model.taskTreeItem( task2_2.id() ).childCount() == 2 );
    QVERIFY( model.taskTreeItem( task2.id() ).childCount() == 2 );

    // now the whole game in reverse: remove tasks, one by one:
    model.deleteTask( task2_2_2 );
    QVERIFY( model.taskTreeItem( task2_2.id() ).childCount() == 1 );
    QVERIFY( model.taskTreeItem( task2.id() ).childCount() == 2 );
    model.deleteTask( task2_2_1 );
    QVERIFY( model.taskTreeItem( task2_2.id() ).childCount() == 0 );
    QVERIFY( model.taskTreeItem( task2.id() ).childCount() == 2 );
    model.deleteTask( task2_1_1 );
    QVERIFY( model.taskTreeItem( task2_1.id() ).childCount() == 1 );
    QVERIFY( model.taskTreeItem( task2.id() ).childCount() == 2 );
    model.deleteTask( task2_1_2 );
    QVERIFY( model.taskTreeItem( task2_1.id() ).childCount() == 0 );
    QVERIFY( model.taskTreeItem( task2.id() ).childCount() == 2 );
    model.deleteTask( task2_2 );
    QVERIFY( model.taskTreeItem( task2.id() ).childCount() == 1 );
    model.deleteTask( task2_1 );
    QVERIFY( model.taskTreeItem( task2.id() ).childCount() == 0 );
    model.deleteTask( task2 );
    QVERIFY( model.taskTreeItem( 0 ).childCount() == 1 );
    model.deleteTask( task1_2 );
    QVERIFY( model.taskTreeItem( task1.id() ).childCount() == 2 );
    model.deleteTask( task1_3 );
    QVERIFY( model.taskTreeItem( task1.id() ).childCount() == 1 );
    model.deleteTask( task1_1 );
    QVERIFY( model.taskTreeItem( task1.id() ).childCount() == 0 );
    QVERIFY( model.taskTreeItem( 0 ).childCount() == 1 );
    model.deleteTask( task1 );
    QVERIFY( model.taskTreeItem( 0 ).childCount() == 0 );
}

void CharmDataModelTests::modifyTaskTest()
{
    CharmDataModel model;
    Task task1( 1000, "Task 1" );
    Task task1_1( 1001, "Task 1-1", task1.id() );
    Task task1_2( 1002, "Task 1-2", task1.id() );
    Task task1_3( 1003, "Task 1-3", task1.id() );
    model.addTask( task1 );
    model.addTask( task1_3 );
    model.addTask( task1_1 );
    model.addTask( task1_2 );
    QVERIFY( model.taskTreeItem( 0 ).childCount() == 1 );
    QVERIFY( model.taskTreeItem( task1.id() ).childCount() == 3 );

    // new values:
    Task task1b( task1 );
    task1b.setName( "Task 1, modified" );
    Task task1_1b( task1_1 );
    task1_1b.setParent( 0 );

    QVERIFY( model.taskTreeItem( task1.id() ).task() == task1 );
    model.modifyTask( task1b );
    QVERIFY( model.taskTreeItem( task1.id() ).task() == task1b );

    QVERIFY( model.taskTreeItem( task1_1.id() ).task() == task1_1 );
    QVERIFY( model.taskTreeItem( 0 ).childCount() == 1 );
    model.modifyTask( task1_1b ); // now a top level
    QVERIFY( model.taskTreeItem( task1_1.id() ).task() == task1_1b );
    QVERIFY( model.taskTreeItem( 0 ).childCount() == 2 );

    model.clearTasks();
    QVERIFY( model.taskTreeItem( 0 ).childCount() == 0 );
}

void CharmDataModelTests::cleanupTestCase ()
{
    m_referenceModel->clearTasks();
    QVERIFY( m_referenceModel->taskTreeItem( 0 ).childCount() == 0 );

    delete m_referenceModel; m_referenceModel = nullptr;
}

QTEST_MAIN( CharmDataModelTests )

#include "moc_CharmDataModelTests.cpp"
