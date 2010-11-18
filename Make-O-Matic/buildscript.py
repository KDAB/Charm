#!/usr/bin/env python

# This file is part of Charm.
# -*- coding: utf-8 -*-
# 
# Copyright (C) 2010 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
# Author: Mirko Boehm <mirko@kdab.com>

from core.helpers.BoilerPlate import getBuildProject
from core.Configuration import Configuration
from core.plugins.Preprocessor import Preprocessor
from core.helpers.PathResolver import PathResolver
from core.plugins.RSyncPublisher import RSyncPublisher
from core.plugins.builders.generators.CMakeBuilder import CMakeBuilder, CMakeVariable
from core.plugins.packagers.CPack import CPack
from core.plugins.testers.CTest import CTest
from core.environments.Environments import Environments
from core.plugins.DoxygenGenerator import DoxygenGenerator
import socket

build, project = getBuildProject( buildName = 'Charm Build', projectName = 'Charm',
								projectVersionNumber = '1.4.0', scmUrl = 'git://github.com/KDAB/Charm.git' )

sharedDebug = Environments( [ 'Qt-4.[67].?-Shared-Debug' ], 'Qt 4 Shared Debug', project )
debug = Configuration( 'Debug', sharedDebug, )
cmakeDebug = CMakeBuilder()
cmakeDebug.addCMakeVariable( CMakeVariable( 'CHARM_TIMESHEET_TOOLS', 'TRUE', 'BOOL' ) )
debug.addPlugin( CTest() )
debug.addPlugin( cmakeDebug )

sharedRelease = Environments( [ 'Qt-4.[67].?-Shared-Release' ], 'Qt 4 Shared Release', project )
release = Configuration( 'Release', sharedRelease )
release.addPlugin( CMakeBuilder() )
release.addPlugin( CTest() )
release.addPlugin( CPack( licenseFile="License.txt" ) )

# publish doxygen documentation:
prep = Preprocessor( project, inputFilename = PathResolver( project.getSourceDir, 'doxygen.cfg.in' ),
					 outputFilename = PathResolver( project.getTempDir, 'doxygen.cfg' ) )
project.addPlugin( prep )
footer = Preprocessor( project, inputFilename = PathResolver( project.getSourceDir, 'doxygen-footer.html.in' ),
					 outputFilename = PathResolver( project.getTempDir, 'doxygen-footer.html' ) )
project.addPlugin( footer )

gen = DoxygenGenerator()
gen.setOptional( True )
gen.setDoxygenFile( prep.getOutputFilename() )
project.addPlugin( gen )
project.addPlugin( RSyncPublisher( localDir = PathResolver( project.getDocsDir ),
	uploadLocation = 'docs.kdab.com:/home/klaralv-web/docs.kdab.net/charm' ) )

# publish packages:
project.addPlugin( RSyncPublisher( localDir = PathResolver( project.getPackagesDir ),
	uploadLocation = 'docs.kdab.com:/home/klaralv-web/docs.kdab.net/charm' ) )

build.getSettings().setBuildStepEnabled( 'conf-package', 'c', True )
# this is not extremely elegant, and will be merged into the main configuration later:
if socket.gethostname() == 'bigmac.office-berlin.kdab.com':
	build.getSettings().setBuildStepEnabled( 'project-upload-packages', 'c', True )

build.build()
