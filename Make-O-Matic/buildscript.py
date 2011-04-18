#!/usr/bin/env python

# This file is part of Charm.
# -*- coding: utf-8 -*-
# 
# Copyright (C) 2011 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
# Author: Mirko Boehm <mirko.boehm@kdab.com>

from core.helpers.BoilerPlate import BuildProject
from core.Configuration import Configuration
from core.plugins.Preprocessor import Preprocessor
from core.helpers.PathResolver import PathResolver
from core.plugins.builders.generators.CMakeBuilder import CMakeBuilder, CMakeVariable
from core.plugins.packagers.CPack import CPack
from core.plugins.testers.CTest import CTest
from core.environments.Environments import Environments
from core.plugins.DoxygenGenerator import DoxygenGenerator

build, project = BuildProject( name = 'Charm', version = '1.5.0', url = 'git://github.com/KDAB/Charm.git' )

sharedDebug = Environments( [ 'Qt-4.[67].?-Shared-Debug' ], 'Qt 4 Shared Debug', project )
sharedDebug.setOptional( True )
debug = Configuration( 'Debug', sharedDebug, )
cmakeDebug = CMakeBuilder()
cmakeDebug.addCMakeVariable( CMakeVariable( 'CHARM_TIMESHEET_TOOLS', 'TRUE', 'BOOL' ) )
cmakeDebug.addCMakeVariable( CMakeVariable( 'CMAKE_BUILD_TYPE', 'debug', 'STRING' ) )
debug.addPlugin( CTest() )
debug.addPlugin( cmakeDebug )

sharedRelease = Environments( [ 'Qt-4.[67].?' ], 'Qt 4 Shared Release', project )
release = Configuration( 'Release', sharedRelease )
cmakeRelease = CMakeBuilder()
cmakeRelease.addCMakeVariable( CMakeVariable( 'CMAKE_BUILD_TYPE', 'release', 'STRING' ) )
release.addPlugin( cmakeRelease )
release.addPlugin( CTest() )
release.addPlugin( CPack( licenseFile = "License.txt" ) )

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

build.build()
