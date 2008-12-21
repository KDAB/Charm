#!/usr/bin/python
import copy, os, platform
from AutobuildCore.Configuration import Configuration
from AutobuildCore.Project import Project

Product = Project( 'Charm' )
# Product.setScmUrl( 'svn://anonsvn.kde.org/home/kde/trunk/playground/utils/charm/trunk' )
Product.setScmUrl( 'svn://kde.codeyard.cs.ru.nl/home/kde/trunk/playground/utils/charm/trunk' )
Product.setScmSrcDir( '' )
Product.getSettings().setCommitterName( 'mlaurent', 'laurent@kdab.net' )

Debug = Configuration( Product, 'Debug' )
Debug.setBuilder( 'cmake' )
Debug.addPackageDependency( 'Qt-4.[4-9].?' )
Debug.addBlacklistedPlatform( 'win32-msvc' ) # disable MSVC6
Debug.setOptions( '-D CMAKE_BUILD_TYPE=debug' )

Release = copy.copy( Debug, )
Release.setConfigName( 'Release' )
Release.setOptions( '-D CMAKE_BUILD_TYPE=release' )

Product.build( [Debug, Release] )
