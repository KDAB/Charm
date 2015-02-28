/*
  BackendIntegrationTests.h

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

#ifndef BACKENDINTEGRATIONTESTS_H
#define BACKENDINTEGRATIONTESTS_H

#include "TestApplication.h"
#include "Core/Task.h"

class BackendIntegrationTests : public TestApplication
{
    Q_OBJECT

public:
    BackendIntegrationTests();

private slots:
    void initTestCase ();

    void initialValuesTest();

    void simpleCreateModifyDeleteTaskTest();

    void biggerCreateModifyDeleteTaskTest();

    void cleanupTestCase ();

private:
    const TaskList& referenceTasks();
    // returns true if both lists contain the same tasks, even if not
    // in the same order:
    bool contentsEqual( const TaskList& list1, const TaskList& list2 );
};

#endif
