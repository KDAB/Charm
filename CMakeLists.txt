# this is the CMake Project File for Charm
PROJECT( Charm )
IF( NOT Charm_VERSION )
   SET( Charm_VERSION_MAJOR "1" )
   SET( Charm_VERSION_MINOR "5" )
   SET( Charm_VERSION_PATCH "0-master" )
   SET( Charm_VERSION_COUNT 3 )
   SET( Charm_VERSION "${Charm_VERSION_MAJOR}.${Charm_VERSION_MINOR}.${Charm_VERSION_PATCH}" )
ENDIF( NOT Charm_VERSION )

IF( NOT ${CMAKE_BUILD_TYPE} )
   SET( CMAKE_BUILD_TYPE "Release" )
ENDIF( NOT ${CMAKE_BUILD_TYPE} )

SET(QT_MIN_VERSION "4.4.0")
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
FIND_PACKAGE( Qt4 REQUIRED )
ENABLE_TESTING()

# Always include srcdir and builddir in include path
# This saves typing ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY} in
# about every subdir since cmake 2.4.0
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# put the include dirs which are in the source or build tree
# before all other include dirs, so the headers in the sources
# are prefered over the already installed ones
# since cmake 2.4.1
set(CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON)

# GCC compiler settings:
IF (CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS( -Wall -Wundef -Wcast-align -Wchar-subscripts -Wpointer-arith 
       -Wwrite-strings -Wpacked -Wformat-security -Wmissing-format-attribute 
       -Wold-style-cast )
    ADD_DEFINITIONS( -fvisibility=hidden )
# to be added:       
#	-Woverloaded-virtual  -Wshadow 
ENDIF (CMAKE_COMPILER_IS_GNUCXX)

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

IF( UNIX AND NOT APPLE )
   SET( DOC_INSTALL_DIR share/doc/charm/ )
ELSE( UNIX AND NOT APPLE )
   SET( DOC_INSTALL_DIR . )
ENDIF( UNIX AND NOT APPLE )

INSTALL( FILES ReadMe.txt License.txt DESTINATION ${DOC_INSTALL_DIR} )

SET( CPACK_GENERATOR "TBZ2" )
SET( CPACK_PACKAGE_VERSION_MAJOR "${Charm_VERSION_MAJOR}" )
SET( CPACK_PACKAGE_VERSION_MINOR "${Charm_VERSION_MINOR}" )
SET( CPACK_PACKAGE_VERSION_PATCH "${Charm_VERSION_PATCH}" )
SET( CPACK_PACKAGE_VERSION "${Charm_VERSION}" )
SET( CPACK_PACKAGE_VENDOR "KDAB" )
SET( CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/ReadMe.txt" )
IF( WIN32 )
    SET( CPACK_GENERATOR "NSIS" "ZIP" )
    SET( CPACK_PACKAGE_EXECUTABLES "Charm" "Charm" )
    SET( CPACK_PACKAGE_INSTALL_DIRECTORY "Charm" )
    SET( CPACK_PACKAGE_FILE_NAME "Charm ${Charm_VERSION}" )
    SET( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/License.txt" )
    SET( CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/Charm/Icons/Charm.ico" )
    SET( CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/Charm/Icons/NSIS-Uninstall.ico" )
    SET( CPACK_NSIS_URL_INFO_ABOUT "http://www.kdab.com/" )
    SET( CPACK_NSIS_INSTALLED_ICON_NAME "Charm.exe" )
    SET( CPACK_NSIS_MENU_LINKS "Charm.exe" "Charm" )
    SET( CPACK_NSIS_DEFINES "!define MUI_FINISHPAGE_RUN \\\"$INSTDIR\\\\Charm.exe\\\" " )
ENDIF( WIN32 )
IF( APPLE )
    SET( CPACK_GENERATOR "DragNDrop" "ZIP" )
    SET( CPACK_DMG_FORMAT "UDBZ" )
    SET( CPACK_DMG_VOLUME_NAME "Charm" )
    SET( CPACK_SYSTEM_NAME "OSX" )
    SET( CPACK_PACKAGE_FILE_NAME "Charm-${Charm_VERSION}" )
    SET( CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/Charm/Icons/CharmDMG.icns" )
ENDIF( APPLE )

INCLUDE( CPack )
