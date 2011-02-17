PROJECT( Charm )
IF( NOT Charm_VERSION )
        SET( Charm_VERSION_MAJOR "1" )
        SET( Charm_VERSION_MINOR "5" )
        SET( Charm_VERSION_PATCH "0-master" )
        SET( Charm_VERSION_COUNT 3 )
        SET(
                Charm_VERSION
                "${Charm_VERSION_MAJOR}.${Charm_VERSION_MINOR}.${Charm_VERSION_PATCH}"
        )
ENDIF()

IF( NOT CMAKE_BUILD_TYPE )
        SET( CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE )
ENDIF()

MESSAGE( STATUS "Building Charm ${Charm_VERSION} in ${CMAKE_BUILD_TYPE} mode" )

CMAKE_MINIMUM_REQUIRED( VERSION 2.6.0 )
FIND_PACKAGE( Qt4 4.6.0 COMPONENTS QtMain QtCore QtGui QtSql QtXml REQUIRED )
INCLUDE( UseQt4 )
ENABLE_TESTING()

IF( CMAKE_BUILD_TYPE MATCHES "^[Rr]elease$" )
        ADD_DEFINITIONS( -DQT_NO_DEBUG_OUTPUT )
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

IF( NOT APPLE )
    INSTALL( FILES ReadMe.txt License.txt DESTINATION ${DOC_INSTALL_DIR} )
ENDIF()

# Only support packaging on newer versions of CMake.
IF( NOT "${CMAKE_VERSION}" VERSION_LESS "2.8.4" )
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
                SET( CPACK_NSIS_EXECUTABLES_DIRECTORY "${BIN_INSTALL_DIR}" )
                SET( CPACK_NSIS_MUI_ICON "${ICONS_DIR}/Charm.ico" )
                SET( CPACK_PACKAGE_ICON "${ICONS_DIR}\\\\CharmNSISHeader.bmp" )
                SET( CPACK_NSIS_URL_INFO_ABOUT "http://www.kdab.com/" )
                SET( CPACK_NSIS_INSTALLED_ICON_NAME "Charm.exe" )
                SET( CPACK_NSIS_MENU_LINKS
                    "License.txt" "License"
                    "ReadMe.txt" "Readme"
                )
                SET( CPACK_NSIS_MUI_FINISHPAGE_RUN "Charm.exe" )
        ENDIF()
        IF( APPLE )
                SET( CMAKE_INSTALL_PREFIX "/Applications" )
                SET( CPACK_GENERATOR "DragNDrop" )
                SET( CPACK_DMG_FORMAT "UDBZ" )
                SET( CPACK_DMG_VOLUME_NAME "Charm" )
                SET( CPACK_SYSTEM_NAME "OSX" )
                SET( CPACK_PACKAGE_FILE_NAME "Charm-${Charm_VERSION}" )
                SET( CPACK_PACKAGE_ICON "${ICONS_DIR}/CharmDMG.icns" )
                SET( CPACK_DMG_DS_STORE "${ICONS_DIR}/CharmDSStore" )
                SET( CPACK_DMG_BACKGROUND_IMAGE "${ICONS_DIR}/CharmDMGBackground.png" )
        ENDIF()

        INCLUDE( CPack )
ENDIF()
