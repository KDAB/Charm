PROJECT( Charm )
CMAKE_MINIMUM_REQUIRED( VERSION 2.8.12 )

include(FeatureSummary)

set(ECM_MODULE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ECM/modules/")
set(CMAKE_MODULE_PATH ${ECM_MODULE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ECM/kde-modules" )

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)

include(ECMInstallIcons)
include(ECMAddAppIcon)


IF( NOT Charm_VERSION )
    FIND_PACKAGE( Git QUIET )
    IF( EXISTS ${GIT_EXECUTABLE} )
        EXECUTE_PROCESS( COMMAND ${GIT_EXECUTABLE} describe --tags --abbrev=1
            RESULT_VARIABLE GIT_RESULT
            OUTPUT_VARIABLE Charm_VERSION
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_STRIP_TRAILING_WHITESPACE )
        IF( NOT GIT_RESULT EQUAL 0 )
            MESSAGE( FATAL_ERROR "Cannot get 'git describe' version!" )
        ENDIF()
    ENDIF()
ENDIF()

STRING( REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.(.*)$" Charm_VERSION "${Charm_VERSION}")
SET( Charm_VERSION_MAJOR "${CMAKE_MATCH_1}" )
SET( Charm_VERSION_MINOR "${CMAKE_MATCH_2}" )
SET( Charm_VERSION_PATCH "${CMAKE_MATCH_3}" )
SET( Charm_VERSION_COUNT 3 )

IF( NOT ( DEFINED Charm_VERSION_MAJOR AND DEFINED Charm_VERSION_MINOR AND DEFINED Charm_VERSION_PATCH ) )
    MESSAGE( FATAL_ERROR "No Git executable or valid Charm version argument found.\n"
    "Please pass a version to CMake e.g. cmake -DCharm_VERSION=1.0.0" )
ENDIF()

MESSAGE( STATUS "Building Charm ${Charm_VERSION} in ${CMAKE_BUILD_TYPE} mode" )

find_package( Qt5Core REQUIRED)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Test REQUIRED)
find_package(Qt5Script REQUIRED)
find_package(Qt5PrintSupport)

if(WIN32)
    find_package(Qt5WinExtras REQUIRED)
endif()

IF(APPLE)
    find_package(Qt5MacExtras REQUIRED)
ENDIF()

IF(UNIX AND NOT APPLE)
    find_package(Qt5DBus QUIET)
    IF(TARGET Qt5::DBus)
        set(HAVE_DBUS ON)
    ENDIF()
ENDIF()



find_package(Qt5Keychain REQUIRED)
set_package_properties(Qt5Keychain PROPERTIES
                                   DESCRIPTION "Provides support for secure credentials storage"
                                   URL "https://github.com/frankosterfeld/qtkeychain"
                                   TYPE REQUIRED)

SET(CHARM_MAC_HIGHRES_SUPPORT_ENABLED ON)


ENABLE_TESTING()

IF( UNIX AND NOT APPLE )
    set( Charm_EXECUTABLE charmtimetracker )
ELSE()
    set( Charm_EXECUTABLE Charm )
ENDIF()

SET( BIN_INSTALL_DIR bin )
SET( DOC_INSTALL_DIR ${CMAKE_INSTALL_DOCBUNDLEDIR}/${Charm_EXECUTABLE} )
SET( ICONS_DIR "${CMAKE_SOURCE_DIR}/Charm/Icons" )

IF( CHARM_PREPARE_DEPLOY AND WIN32 OR APPLE)
SET( BIN_INSTALL_DIR . )
    SET( DOC_INSTALL_DIR . )
ENDIF()

OPTION( CHARM_IDLE_DETECTION "Build the Charm idle detector" ON )
OPTION( CHARM_TIMESHEET_TOOLS "Build the Charm timesheet tools" OFF )
set( CHARM_IDLE_TIME "360" CACHE STRING "Set the idle timeout (in seconds, default 360)" )
OPTION( CHARM_CI_SUPPORT "Build Charm with command interface support" OFF )

IF( CHARM_CI_SUPPORT )
    OPTION( CHARM_CI_TCPSERVER "Build Charm with TCP command interface support" ON )
    OPTION( CHARM_CI_LOCALSERVER "Build Charm with local socket command interface support" ON )
ENDIF()

OPTION(CHARM_PREPARE_DEPLOY "Deploy dependencies with install target(Windows, Apple)" ON)

ADD_SUBDIRECTORY( Core )
ADD_SUBDIRECTORY( Charm )

IF( CHARM_TIMESHEET_TOOLS AND UNIX )
    # Only build the tools if they are explicitly requested to avoid
    # the Qt MySQL driver dependency.
    ADD_SUBDIRECTORY( Tools/TimesheetProcessor )
    ADD_SUBDIRECTORY( Tools/TimesheetGenerator )
    MESSAGE( STATUS "Building the Charm timesheet tools")
ENDIF()

ADD_SUBDIRECTORY( Tests )

CONFIGURE_FILE( CharmCMake.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/CharmCMake.h )

SET( LICENSE_FILE "License.txt" )
SET( README_FILE "ReadMe.txt" )
IF( NOT APPLE )
    INSTALL( FILES "${LICENSE_FILE}" "${README_FILE}" DESTINATION ${DOC_INSTALL_DIR} )
ENDIF()

    # Only support CPack packaging on newer versions of CMake.
IF( NOT "${CMAKE_VERSION}" VERSION_LESS "2.8.4" )
    SET( CPACK_GENERATOR "TBZ2" )
    SET( CPACK_PACKAGE_VERSION_MAJOR "${Charm_VERSION_MAJOR}" )
    SET( CPACK_PACKAGE_VERSION_MINOR "${Charm_VERSION_MINOR}" )
    SET( CPACK_PACKAGE_VERSION_PATCH "${Charm_VERSION_PATCH}" )
    SET( CPACK_PACKAGE_VERSION "${Charm_VERSION}" )
    SET( CPACK_PACKAGE_VENDOR "KDAB" )
    SET( CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/${README_FILE}" )
    IF( WIN32 )
        SET( CPACK_GENERATOR "NSIS" "ZIP" )
        SET( CPACK_PACKAGE_EXECUTABLES "Charm" "Charm" )
        SET( CPACK_PACKAGE_INSTALL_DIRECTORY "Charm" )
        SET( CPACK_PACKAGE_FILE_NAME "Charm ${Charm_VERSION}" )
        SET( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/${LICENSE_FILE}" )
        SET( CPACK_NSIS_EXECUTABLES_DIRECTORY "${BIN_INSTALL_DIR}" )
        SET( CPACK_NSIS_MUI_ICON "${ICONS_DIR}/Charm.ico" )
        SET( CPACK_PACKAGE_ICON "${ICONS_DIR}\\\\CharmNSISHeader.bmp" )
        SET( CPACK_NSIS_URL_INFO_ABOUT "http://www.kdab.com/" )
        SET( CPACK_NSIS_INSTALLED_ICON_NAME "Charm${CMAKE_EXECUTABLE_SUFFIX}" )
        SET( CPACK_NSIS_MENU_LINKS
            "${LICENSE_FILE}" "License"
            "${README_FILE}" "Readme"
        )
    ELSEIF( APPLE )
        SET( CPACK_GENERATOR "DragNDrop" )
        SET( CPACK_DMG_FORMAT "UDBZ" )
        SET( CPACK_DMG_VOLUME_NAME "Charm" )
        SET( CPACK_SYSTEM_NAME "OSX" )
        SET( CPACK_PACKAGE_FILE_NAME "Charm-${Charm_VERSION}" )
        SET( CPACK_PACKAGE_ICON "${ICONS_DIR}/CharmDMG.icns" )
        SET( CPACK_DMG_DS_STORE "${ICONS_DIR}/CharmDSStore" )
        SET( CPACK_DMG_BACKGROUND_IMAGE "${ICONS_DIR}/CharmDMGBackground.png" )
    ELSEIF( UNIX )
        SET( CPACK_SYSTEM_NAME
            "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}"
        )
    ENDIF()

    INCLUDE( CPack )
ENDIF()

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES)
