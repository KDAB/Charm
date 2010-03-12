# this is the CMake Project File for Charm
PROJECT( Charm )
SET( Charm_VERSION_MAJOR "1" )
SET( Charm_VERSION_MINOR "3" )
SET( Charm_VERSION_PATCH "0" )
SET( Charm_VERSION_COUNT 3 )
SET( Charm_VERSION "${Charm_VERSION_MAJOR}.${Charm_VERSION_MINOR}.${Charm_VERSION_PATCH}" )

# Charm uses layout and QPalette elements from Qt 4.4
SET(QT_MIN_VERSION "4.4.0")
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
FIND_PACKAGE( Qt4 REQUIRED )
ENABLE_TESTING()

# TEMP, this will go into a FindAutobuild.cmake:
SET( AUTOBUILD_INFO_AUTOBUILD_VERSION $ENV{AUTOBUILD_INFO_AUTOBUILD_VERSION} )
IF( AUTOBUILD_INFO_AUTOBUILD_VERSION )
    MESSAGE( "Discovered Autobuild $ENV{AUTOBUILD_INFO_AUTOBUILD_VERSION}" )
    SET( AUTOBUILD_INFO_REVISION $ENV{AUTOBUILD_INFO_REVISION} CACHE STRING "Autobuild Revision")
    SET( AUTOBUILD_INFO_SCMURL $ENV{AUTOBUILD_INFO_SCMURL} CACHE STRING "Autobuild SCM URL" )
    SET( AUTOBUILD_INFO_COMMITTER $ENV{AUTOBUILD_INFO_COMMITTER} CACHE STRING "Autobuild Committer Name" )
    SET( AUTOBUILD_INFO_PROJECTNAME_WITH_VERSION $ENV{AUTOBUILD_INFO_PROJECTNAME_WITH_VERSION} CACHE STRING "Autobuild Project Name with Version" )
    SET( AUTOBUILD_INFO_PROJECTNAME $ENV{AUTOBUILD_INFO_PROJECTNAME} CACHE STRING "Autobuild Project Name" )
    SET( AUTOBUILD_INFO_PROJECTVERSION $ENV{AUTOBUILD_INFO_PROJECTVERSION} CACHE STRING "Autobuild Project Version" )
    SET( AUTOBUILD_INFO_BUILDTYPE $ENV{AUTOBUILD_INFO_BUILDTYPE} CACHE STRING "Autobuild Build Type" )
    SET( AUTOBUILD_INFO_STARTTIME $ENV{AUTOBUILD_INFO_STARTTIME} CACHE STRING "Autobuild Build Start Time" )
    SET( AUTOBUILD_INFO_CONFIGURATION_NAME $ENV{AUTOBUILD_INFO_CONFIGURATION_NAME} CACHE STRING "Autobuild Configuration Name" )
    SET( AUTOBUILD_INFO_ENVIRONMENT_NAME $ENV{AUTOBUILD_INFO_ENVIRONMENT_NAME} CACHE STRING "Autobuild Build Environment Name" )
    SET( AUTOBUILD_INFO_BUILDHOST $ENV{AUTOBUILD_INFO_BUILDHOST} CACHE STRING "Autobuild Build Host" )
ELSE()
    SET( AUTOBUILD_INFO_AUTOBUILD_VERSION "not found" CACHE STRING "Autobuild not discovered" )
ENDIF()

#
# NOTE: 
# If you are building against a static Qt, you need to set an extra variable on 
# the command line: CHARM_BUILD_STATIC_QT:BOOL=ON
# 
# we do not set this anymore, turn it of by providing the option on
# the cmake command line ( -DCMAKE_COLOR_MAKEFILE=off )
# SET( CMAKE_COLOR_MAKEFILE off )

# Always include srcdir and builddir in include path
# This saves typing ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY} in
# about every subdir since cmake 2.4.0
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# put the include dirs which are in the source or build tree
# before all other include dirs, so the headers in the sources
# are prefered over the already installed ones
# since cmake 2.4.1
set(CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON)

# some magic to put Subversion information in the code:
# (note: commented out, since this requires a network connection)
# FIND_PACKAGE( Subversion )
#IF( Subversion_FOUND )
#    Subversion_WC_INFO( ${Charm_SOURCE_DIR} Charm )
#    MESSAGE( "Building Revision ${Charm_WC_REVISION} of Charm" )
#    MESSAGE( "Building from ${Charm_WC_URL}" )
#ENDIF( Subversion_FOUND )

# GCC compiler settings:
IF (CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS( -Wall -Wundef -Wcast-align -Wchar-subscripts -Wpointer-arith 
       -Wwrite-strings -Wpacked -Wformat-security -Wmissing-format-attribute 
       -Wold-style-cast )
    ADD_DEFINITIONS( -fvisibility=hidden )
# to be added:       
#	-Woverloaded-virtual  -Wshadow 
# FIXME we might want this one back in, but Qt 4.4.3 spits gazillions of warnings with it on Linux-64:
#       -Wconversion
#  -Wnon-virtual-dtor # Qt has too many of those already
ENDIF (CMAKE_COMPILER_IS_GNUCXX)

IF( CHARM_BUILD_STATIC_QT )
    ADD_DEFINITIONS( -DCHARM_BUILD_STATIC_QT )
    IF( UNIX )
        IF( NOT APPLE )
	    # specify the extra libraries, they are not specified in the Qt4 module because the 
	    # Qt libs are static
	    FIND_PACKAGE( JPEG REQUIRED ) 
	    FIND_PACKAGE( Freetype REQUIRED ) 
	    FIND_PACKAGE( X11 REQUIRED ) 
	    FIND_PACKAGE( JPEG REQUIRED ) 
	    FIND_PACKAGE( PNG REQUIRED ) 

	    FIND_LIBRARY( FONT_CONFIG_LIB NAMES fontconfig PATH /usr/lib )
	    IF( FONT_CONFIG_LIB ) 
		MESSAGE( "Found Fontconfig" )
		LINK_LIBRARIES( ${FONT_CONFIG_LIB} )
	    ELSE( FONT_CONFIG_LIB )
		MESSAGE(FATAL_ERROR "Fontconfig library not found" )
	    ENDIF( FONT_CONFIG_LIB ) 
	
	    LINK_LIBRARIES( ${JPEG_LIBRARY} )
	    LINK_LIBRARIES( ${X11_SM_LIB} )
	    LINK_LIBRARIES( ${X11_ICE_LIB} )
	    LINK_LIBRARIES( ${PNG_LIBRARY} )
	    LINK_LIBRARIES( ${FREETYPE_LIBRARY} )
	    LINK_LIBRARIES( ${X11_Xrender_LIB} )
	    LINK_LIBRARIES( ${X11_Xrandr_LIB} )
	    LINK_LIBRARIES( ${X11_Xinput_LIB} )
	    LINK_LIBRARIES( ${QT_QTXML_LIBRARY} )
	    LINK_LIBRARIES( ${QT_X11_Xext_LIBRARY} )
      
	    # FIXME why does FIND_PACKAGE( GTK ... ) not find this?
	    LINK_LIBRARIES( glib-2.0 )
	    LINK_LIBRARIES( gobject-2.0 )
	    LINK_LIBRARIES( gthread-2.0 )
	ENDIF( NOT APPLE )
    ENDIF( UNIX )
ENDIF( CHARM_BUILD_STATIC_QT )    

ADD_SUBDIRECTORY( Core )
ADD_SUBDIRECTORY( Charm )

# the tools are only built if they are explicitly enabled, to avoid the Qt MySQL driver dependency
IF( CHARM_ENABLE_TOOLS_BUILD )
    IF(UNIX)
	ADD_SUBDIRECTORY( Tools/TimesheetProcessor )
	ADD_SUBDIRECTORY( Tools/TimesheetGenerator )
	MESSAGE( "This is Unix, building Charm Tools")
    ENDIF(UNIX)
ELSE( CHARM_ENABLE_TOOLS_BUILD )
    MESSAGE( "Charm Tools Build is disabled" )
ENDIF( CHARM_ENABLE_TOOLS_BUILD )

ADD_SUBDIRECTORY( Tests )

SET( CPACK_GENERATOR "ZIP" )
SET( CPACK_PACKAGE_VERSION_MAJOR "${Charm_VERSION_MAJOR}" )
SET( CPACK_PACKAGE_VERSION_MINOR "${Charm_VERSION_MINOR}" )
SET( CPACK_PACKAGE_VERSION_PATCH "${Charm_VERSION_PATCH}" )
SET( CPACK_PACKAGE_VERSION "${Charm_VERSION}" )
IF( APPLE )
    SET( CPACK_SYSTEM_NAME "OSX" )
ENDIF ( APPLE )

INCLUDE( CPack )
