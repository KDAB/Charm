CMAKE_MINIMUM_REQUIRED( VERSION 2.8.12 )
PROJECT( Charm CXX )

include(FeatureSummary)

set(ECM_MODULE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ECM/modules/")
set(CMAKE_MODULE_PATH ${ECM_MODULE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ECM/kde-modules" )

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)

include(ECMInstallIcons)


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

add_definitions(-DCHARM_VERSION="${Charm_VERSION}")

find_package( Qt5Core REQUIRED)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Test REQUIRED)
find_package(Qt5PrintSupport)

if(WIN32)
    find_package(Qt5WinExtras REQUIRED)
endif()

IF(APPLE)
    find_package(Qt5MacExtras REQUIRED)
ENDIF()

IF(UNIX AND NOT APPLE)
    find_package(Qt5DBus QUIET)
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
SET( README_FILE "ReadMe.md" )
IF( NOT APPLE )
    INSTALL( FILES "${LICENSE_FILE}" "${README_FILE}" DESTINATION ${DOC_INSTALL_DIR} )
ENDIF()

IF (WIN32)
    FIND_PACKAGE( PythonInterp 3.5 QUIET)
    set_package_properties(PythonInterp PROPERTIES
                                        PURPOSE "Used to create Windows installer"
                                        TYPE OPTIONAL)


    IF(PYTHONINTERP_FOUND)
        OPTION(CHARM_SIGN_INSTALLER "Sign the installer and the contained files" ON)

        SET(EXTRA_PACKAGE_COMMANDS )
        FIND_PACKAGE( OpenSSL QUIET)
        set_package_properties(OpenSSL PROPERTIES
                                       PURPOSE "Deployment of openssl libraries."
                                       TYPE OPTIONAL)

        IF (OPENSSL_FOUND)
            SET(EXTRA_PACKAGE_COMMANDS ${EXTRA_PACKAGE_COMMANDS} --deployOpenSSL "${OPENSSL_INCLUDE_DIR}/../")
        ENDIF()
        IF( CMAKE_BUILD_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])" )
            SET(EXTRA_PACKAGE_COMMANDS ${EXTRA_PACKAGE_COMMANDS} --buildType debug)
        ENDIF()

        IF (CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(EXTRA_PACKAGE_COMMANDS ${EXTRA_PACKAGE_COMMANDS} --architecture x64)
        ELSE()
            set(EXTRA_PACKAGE_COMMANDS ${EXTRA_PACKAGE_COMMANDS} --architecture x86)
        ENDIF()

        IF(CHARM_SIGN_INSTALLER)
            set(EXTRA_PACKAGE_COMMANDS ${EXTRA_PACKAGE_COMMANDS} --sign)
        ENDIF()


        ADD_CUSTOM_TARGET(package
            COMMAND
                ${PYTHON_EXECUTABLE}
                "${CMAKE_CURRENT_SOURCE_DIR}/scripts/create-win-installer.py"
                --installerName "Charm-${Charm_VERSION}.exe"
                --applicationFileName "bin/Charm.exe"
                --buildDir "${CMAKE_CURRENT_BINARY_DIR}"
                --productName Charm
                --productVersion "${Charm_VERSION}"
                --companyName KDAB
                --applicationIcon "${CMAKE_CURRENT_SOURCE_DIR}/Charm/Icons/Charm.ico"
                --productLicence "${CMAKE_CURRENT_SOURCE_DIR}/License.txt"
                ${EXTRA_PACKAGE_COMMANDS}
            DEPENDS
                ${Charm_EXECUTABLE}
            VERBATIM)
    ENDIF()
ENDIF()

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES)
