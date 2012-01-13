PROJECT( Charm )
IF( NOT Charm_VERSION )
    FIND_PACKAGE( Git QUIET )
    IF( EXISTS ${GIT_EXECUTABLE} )
        EXECUTE_PROCESS( COMMAND ${GIT_EXECUTABLE} describe --tags --abbrev=1
                         RESULT_VARIABLE GIT_RESULT
                         OUTPUT_VARIABLE Charm_VERSION
                         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} )
        STRING( STRIP "${Charm_VERSION}" Charm_VERSION )
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

IF( NOT ( Charm_VERSION_MAJOR AND Charm_VERSION_MINOR AND Charm_VERSION_PATCH ) )
    MESSAGE( FATAL_ERROR "No Git executable or valid Charm version argument found.\n"
             "Please pass a version to CMake e.g. cmake -DCharm_VERSION=1.0.0" )
ENDIF()

IF( NOT CMAKE_BUILD_TYPE )
        SET( CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE )
ENDIF()

IF( APPLE AND CMAKE_INSTALL_PREFIX MATCHES "/usr/local" )
        SET( CMAKE_INSTALL_PREFIX "/Applications" )
ENDIF()

MESSAGE( STATUS "Building Charm ${Charm_VERSION} in ${CMAKE_BUILD_TYPE} mode" )

CMAKE_MINIMUM_REQUIRED( VERSION 2.6.0 )
SET( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake" )
FIND_PACKAGE( Qt4 4.6.0 COMPONENTS QtMain QtCore QtGui QtSql QtXml QtNetwork REQUIRED )
INCLUDE( UseQt4 )
ENABLE_TESTING()

IF( CMAKE_BUILD_TYPE MATCHES "^[Rr]elease$" )
        ADD_DEFINITIONS( -DQT_NO_DEBUG_OUTPUT )
ELSE()
        MESSAGE( WARNING
            "Only run Charm in Debug mode if you are doing development!\n"
            "Things like idle detection behave very differently in Debug mode."
        )
ENDIF()

# Always include the source and build directories in the include path
# to save doing so manually in every subdirectory.
SET( CMAKE_INCLUDE_CURRENT_DIR ON )

# Put the include directories which are in the source or build tree
# before all other include directories so they are prefered over
# any installed Charm headers.
SET( CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON )

IF( CMAKE_COMPILER_IS_GNUCXX )
        # Add additional GCC options.
        ADD_DEFINITIONS(
                -Wall -Wundef -Wcast-align -Wchar-subscripts -Wpointer-arith
                -Wwrite-strings -Wpacked -Wformat-security
                -Wmissing-format-attribute -Wold-style-cast
        )
        ADD_DEFINITIONS( -fvisibility=hidden )
ENDIF()


IF( UNIX AND NOT APPLE )
        SET( BIN_INSTALL_DIR bin )
        SET( DOC_INSTALL_DIR share/doc/charm/ )
ELSE()
        SET( BIN_INSTALL_DIR . )
        SET( DOC_INSTALL_DIR . )
ENDIF()
SET( ICONS_DIR "${CMAKE_SOURCE_DIR}/Charm/Icons" )

OPTION( CHARM_IDLE_DETECTION "Build the Charm idle detector" ON )
OPTION( CHARM_TIMESHEET_TOOLS "Build the Charm timesheet tools" OFF )

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
                SET( CPACK_NSIS_MUI_FINISHPAGE_RUN "${CPACK_NSIS_INSTALLED_ICON_NAME}" )
        ENDIF()
        IF( APPLE )
                SET( CPACK_GENERATOR "DragNDrop" )
                SET( CPACK_DMG_FORMAT "UDBZ" )
                SET( CPACK_DMG_VOLUME_NAME "Charm" )
                SET( CPACK_SYSTEM_NAME "OSX" )
                SET( CPACK_PACKAGE_FILE_NAME "Charm-${Charm_VERSION}" )
                SET( CPACK_PACKAGE_ICON "${ICONS_DIR}/CharmDMG.icns" )
                SET( CPACK_DMG_DS_STORE "${ICONS_DIR}/CharmDSStore" )
                SET( CPACK_DMG_BACKGROUND_IMAGE "${ICONS_DIR}/CharmDMGBackground.png" )
        ENDIF()
        IF( UNIX AND NOT APPLE )
                SET( CPACK_SYSTEM_NAME
                    "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}"
                )
        ENDIF()

        INCLUDE( CPack )
ENDIF()
