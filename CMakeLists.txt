# this is the CMake Project File for Charm
PROJECT( Charm )

# Charm uses layout and QPalette elements from Qt 4.4
SET(QT_MIN_VERSION "4.4.0")
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
FIND_PACKAGE( Qt4 REQUIRED )
ENABLE_TESTING()
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
IF (CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS( -fvisibility=hidden )
ENDIF (CMAKE_COMPILER_IS_GNUCXX)
IF( CHARM_BUILD_STATIC_QT )
    ADD_DEFINITIONS( -DCHARM_BUILD_STATIC_QT )
ENDIF( CHARM_BUILD_STATIC_QT )    

ADD_SUBDIRECTORY( Core )
ADD_SUBDIRECTORY( Charm )

# the tools are only built if they are explicitly enabled, to avoid the Qt MySQL driver dependency
IF( CHARM_ENABLE_TOOLS_BUILD )
    IF(UNIX)
	ADD_SUBDIRECTORY( Tools/TimesheetProcessor )
	MESSAGE( "This is Unix, building Charm Tools")
    ENDIF(UNIX)
ELSE( CHARM_ENABLE_TOOLS_BUILD )
    MESSAGE( "Charm Tools Build is disabled" )
ENDIF( CHARM_ENABLE_TOOLS_BUILD )

ADD_SUBDIRECTORY( Tests )
