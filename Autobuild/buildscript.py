#!/usr/bin/python
import copy, os, platform
from AutobuildCore.Configuration import Configuration
from AutobuildCore.Project import Project
from AutobuildCore.Callbacks.DynamicUploadLocation import BranchNamePackageSubdir
from AutobuildCore.Callbacks.ConfigurationBuildSequence import ConfigurationBuildSequence
from AutobuildCore.Callbacks.BuildInformation import BuildInformation

scmPath = 'svn://kde.codeyard.cs.ru.nl/home/kde/trunk/playground/utils/charm'
Product = Project( 'Charm', '0.4.2' )
Product.setScmUrl( scmPath + '/trunk' )
Product.setScmSrcDir( '' )
Product.addCallback( BranchNamePackageSubdir( scmPath ) )

Debug = Configuration( Product, 'Shared Debug' )
Debug.addCallback( ConfigurationBuildSequence( 'conf-bin-package', '*Static*', False, False ) ) # do not package shared builds
Debug.addCallback( BuildInformation() )
Debug.setBuilder( 'cmake' )
Debug.addPackageDependency( 'Qt-4.[4-9].?-Shared-Debug' )
Debug.addBlacklistedPlatform( 'win32-msvc' ) # disable MSVC6
Debug.setOptions( '-D CMAKE_BUILD_TYPE=debug -D CHARM_ENABLE_TOOLS_BUILD:BOOL=true' )

Release = copy.copy( Debug, )
Release.setConfigName( 'Shared Release' )
Release.setPackageDependencies( [ 'Qt-4.[4-9].?-Shared-Release' ] )
Release.setOptions( '-D CMAKE_BUILD_TYPE=release -D CHARM_ENABLE_TOOLS_BUILD:BOOL=true' )

StaticRelease = copy.copy( Debug )
StaticRelease.setConfigName( 'Static Release' )
StaticRelease.setPackageDependencies( [ 'Qt-4.[4-9].?-Static-Release' ] )
StaticRelease.setOptions( '-D CMAKE_BUILD_TYPE=release -D CHARM_BUILD_STATIC_QT:BOOL=true' )

# a configuration that does not use any Autobuild environments
PlatformBuild = Configuration( Product, "Platform Build" )
PlatformBuild.setBuilder( 'cmake' )
PlatformBuild.setOptions( '-D CMAKE_BUILD_TYPE=release' )

# platform build is not enabled by default: 
Product.build( [ Debug, Release, StaticRelease ] )
