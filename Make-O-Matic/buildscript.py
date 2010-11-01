#!/usr/bin/env python

# This file is part of Charm.
# -*- coding: utf-8 -*-
# 
# Copyright (C) 2010 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
# Author: Mirko Boehm <mirko@kdab.com>

from core.helpers.BoilerPlate import getBuildProject
from core.Configuration import Configuration
from core.helpers.PathResolver import PathResolver
from core.plugins.RSyncPublisher import RSyncPublisher
from core.plugins.builders.generators.CMakeBuilder import CMakeBuilder, CMakeVariable
from core.plugins.packagers.CPack import CPack
from core.plugins.testers.CTest import CTest
from core.environments.Environments import Environments

build, project = getBuildProject( buildName = 'Charm Build', projectName = 'Charm',
								projectVersionNumber = '1.4.0', scmUrl = 'git://github.com/KDAB/Charm.git' )

sharedDebug = Environments( [ 'Qt-4.[67].?-Shared-Debug' ], 'Qt 4 Shared Debug', project )
debug = Configuration( 'Debug', sharedDebug, )
cmakeDebug = CMakeBuilder()
cmakeDebug.addCMakeVariable( CMakeVariable( 'CHARM_ENABLE_TOOLS_BUILD', 'TRUE', 'BOOL' ) )
debug.addPlugin( CTest() )
debug.addPlugin( cmakeDebug )

sharedRelease = Environments( [ 'Qt-4.[67].?-Shared-Release' ], 'Qt 4 Shared Release', project )
release = Configuration( 'Release', sharedRelease )
release.addPlugin( CMakeBuilder() )
release.addPlugin( CTest() )
release.addPlugin( CPack() )

# add a RSync publisher (remember to set the default upload location in the configuration file!):
project.addPlugin( RSyncPublisher( localDir = PathResolver( project.getPackagesDir ) ) )

build.build()
