# this is the CMake Project File for Charm
PROJECT( Charm )

FIND_PACKAGE( Qt4 REQUIRED )
ENABLE_TESTING()
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

ADD_SUBDIRECTORY( Core )
ADD_SUBDIRECTORY( View )
ADD_SUBDIRECTORY( Charm )
ADD_SUBDIRECTORY( Tests )
