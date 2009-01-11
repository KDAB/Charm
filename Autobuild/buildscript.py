#!/usr/bin/python
import copy, os, platform
from AutobuildCore.Configuration import Configuration
from AutobuildCore.Project import Project

scmPath = 'svn://kde.codeyard.cs.ru.nl/home/kde/trunk/playground/utils/charm'
Product = Project( 'Charm', '0.4.2' )
Product.setScmUrl( scmPath + '/trunk' )
Product.setScmSrcDir( '' )

# add a callback to manipulate the configuration build sequence at runtime:
def preBuildJobHook( buildJob ):
	if 'Static' not in buildJob.configuration().getConfigName():
		step = buildJob.executomat().step( 'conf-bin-package' )
		step.setEnabled( False ) # only package up static builds

Debug = Configuration( Product, 'Debug' )
Debug.setPreBuildHook( preBuildJobHook )  
Debug.setBuilder( 'cmake' )
Debug.addPackageDependency( 'Qt-4.[4-9].?' )
Debug.addBlacklistedPlatform( 'win32-msvc' ) # disable MSVC6
Debug.setOptions( '-D CMAKE_BUILD_TYPE=debug -D CHARM_ENABLE_TOOLS_BUILD:BOOL=true' )

Release = copy.copy( Debug, )
Release.setConfigName( 'Release' )
Release.setOptions( '-D CMAKE_BUILD_TYPE=release -D CHARM_ENABLE_TOOLS_BUILD:BOOL=true' )

StaticRelease = copy.copy( Debug )
StaticRelease.setConfigName( 'Static Release' )
StaticRelease.setPackageDependencies( [ 'Qt-4.[4-9].?-Static' ] )
StaticRelease.setOptions( '-D CMAKE_BUILD_TYPE=release -D CHARM_BUILD_STATIC_QT:BOOL=true' )

# add a callback to manipulate the package location depending on the branch: 
def preSetupCallback( project ):
	if len( scmPath ) < len( project.getScmUrl() ):
		project.setPackageSubDir(project.getScmUrl()[len( scmPath ):])
Product.registerPresetupCallback(preSetupCallback)

Product.build( [ Debug, Release, StaticRelease] )
