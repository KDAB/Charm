PROJECT( Charm )
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

IF( NOT CMAKE_BUILD_TYPE )
    SET( CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE )
ENDIF()

IF( APPLE AND CMAKE_INSTALL_PREFIX MATCHES "/usr/local" )
    SET( CMAKE_INSTALL_PREFIX "/Applications" )
ENDIF()

MESSAGE( STATUS "Building Charm ${Charm_VERSION} in ${CMAKE_BUILD_TYPE} mode" )

CMAKE_MINIMUM_REQUIRED( VERSION 2.6.0 )
SET( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake" )

if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
endif()

SET( CMAKE_AUTOMOC ON )

OPTION( CHARM_FORCE_QT4 "Force building Charm with Qt4" OFF )

# try Qt5 first, and prefer that if found
IF( NOT CHARM_FORCE_QT4 )
    find_package( Qt5Core QUIET )
ENDIF()

IF(Qt5Core_FOUND)
    find_package(Qt5Widgets REQUIRED)
    find_package(Qt5Xml REQUIRED)
    find_package(Qt5Network REQUIRED)
    find_package(Qt5Sql REQUIRED)
    find_package(Qt5Test REQUIRED)

    find_package(Qt5PrintSupport)
    IF(Qt5PrintSupport_FOUND)
        include_directories(${Qt5PrintSupport_INCLUDE_DIRS})
        set(QTPRINT_LIBRARIES ${Qt5PrintSupport_LIBRARIES})
    ENDIF()

    SET(CHARM_MAC_HIGHRES_SUPPORT_ENABLED ON)

    IF(WIN32)
        SET(QT_PLATFORM_SPECIFIC_LIBRARIES Qt5::WinMain)
    ELSE()
        SET(QT_PLATFORM_SPECIFIC_LIBRARIES)
    ENDIF()

    IF(APPLE)
        find_package(Qt5MacExtras REQUIRED)
        include_directories(${Qt5MacExtras_INCLUDE_DIRS})
        set(QT_PLATFORM_SPECIFIC_LIBRARIES ${Qt5MacExtras_LIBRARIES})
    ENDIF()

    IF(UNIX AND NOT APPLE)
        find_package(Qt5DBus)
        IF(Qt5DBus_FOUND)
            set(HAVE_DBUS ON)
            include_directories(${Qt5DBus_INCLUDE_DIRS})
            set(QTDBUS_LIBRARIES ${Qt5DBus_LIBRARIES})
            macro(qt_add_dbus_interface)
                qt5_add_dbus_interface(${ARGN})
            endmacro()
        ELSE()
            set(QTDBUS_LIBRARIES "")
            macro(qt_add_dbus_interface)
                # do nothing
            endmacro()
        ENDIF()
    ENDIF()

    macro(qt_wrap_cpp)
        qt5_wrap_cpp(${ARGN})
    endmacro()
    macro(qt_wrap_ui)
        qt5_wrap_ui(${ARGN})
    endmacro()
    macro(qt_add_resources)
        qt5_add_resources(${ARGN})
    endmacro()

    #set(QTCORE_LIBRARIES ${Qt5Core_LIBRARIES})
    set(QT_QTTEST_LIBRARY ${Qt5Test_LIBRARIES})

    set(QT_LIBRARIES
        ${Qt5Core_LIBRARIES}
        ${Qt5Widgets_LIBRARIES}
        ${Qt5Xml_LIBRARIES}
        ${Qt5Network_LIBRARIES}
        ${Qt5Sql_LIBRARIES}
        ${QTPRINT_LIBRARIES}
        ${QTDBUS_LIBRARIES}
        ${QT_PLATFORM_SPECIFIC_LIBRARIES}
    )

    include_directories(
        ${Qt5Core_INCLUDE_DIRS}
        ${Qt5Widgets_INCLUDE_DIRS}
        ${Qt5Xml_INCLUDE_DIRS}
        ${Qt5Network_INCLUDE_DIRS}
        ${Qt5Sql_INCLUDE_DIRS}
    )

    SET( CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS} ${CMAKE_CXX_FLAGS}" )
ELSE()
    # No Qt5 found, try Qt4
    find_package(Qt4 COMPONENTS QtMain QtCore QtGui QtSql QtXml QtNetwork REQUIRED)
    INCLUDE( UseQt4 )

    SET(CHARM_MAC_HIGHRES_SUPPORT_ENABLED OFF)

    IF(UNIX AND NOT APPLE)
        find_package(Qt4 COMPONENTS QtDBus)
        IF (QT_QTDBUS_FOUND)
            set(HAVE_DBUS ON)
            set(QTDBUS_LIBRARIES ${QT_QTDBUS_LIBRARY})
            macro(qt_add_dbus_interface)
                qt4_add_dbus_interface(${ARGN})
            endmacro()
        ENDIF()
    ENDIF()

    IF (NOT QT_QTDBUS_FOUND)
        set(QTDBUS_LIBRARIES "")
        macro(qt_add_dbus_interface)
            # do nothing
        endmacro()
    ENDIF()

    include_directories(${QT_INCLUDES})
    #set(QTCORE_LIBRARIES ${QT_QTCORE_LIBRARY})

    macro(qt_wrap_cpp)
        qt4_wrap_cpp(${ARGN})
    endmacro()
    macro(qt_wrap_ui)
        qt4_wrap_ui(${ARGN})
    endmacro()
    macro(qt_add_resources)
        qt4_add_resources(${ARGN})
    endmacro()
ENDIF()

ENABLE_TESTING()

IF( APPLE AND "${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}" VERSION_EQUAL "4.8.0" )
    MESSAGE( WARNING "Qt 4.8.0 is rather broken on OSX.\n"
                     "Known bugs in Charm include global keyboard shortcuts.\n"
                     "Qt 4.7.4 is the current recommended version on OSX." )
ENDIF()

IF( CMAKE_BUILD_TYPE MATCHES "^[Rr]elease$" )
    ADD_DEFINITIONS( -DQT_NO_DEBUG_OUTPUT )
ENDIF()

# Always include the source and build directories in the include path
# to save doing so manually in every subdirectory.
SET( CMAKE_INCLUDE_CURRENT_DIR ON )

# Put the include directories which are in the source or build tree
# before all other include directories so they are preferred over
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
ELSEIF( CMAKE_CXX_COMPILER MATCHES "clang" )
    ADD_DEFINITIONS( -Wall -Wextra -Wno-unused-parameter )
    ADD_DEFINITIONS( -fvisibility=hidden )
ENDIF()


IF( UNIX AND NOT APPLE )
    set( Charm_EXECUTABLE charmtimetracker )
    SET( BIN_INSTALL_DIR bin )
    SET( DOC_INSTALL_DIR share/doc/${Charm_EXECUTABLE} )
ELSE()
    set( Charm_EXECUTABLE Charm )
    SET( BIN_INSTALL_DIR . )
    SET( DOC_INSTALL_DIR . )
ENDIF()
SET( ICONS_DIR "${CMAKE_SOURCE_DIR}/Charm/Icons" )

OPTION( CHARM_IDLE_DETECTION "Build the Charm idle detector" ON )
OPTION( CHARM_TIMESHEET_TOOLS "Build the Charm timesheet tools" OFF )
set( CHARM_IDLE_TIME "360" CACHE STRING "Set the idle timeout (in seconds, default 360)" )
OPTION( CHARM_CI_SUPPORT "Build Charm with command interface support" OFF )

IF( CHARM_CI_SUPPORT )
    OPTION( CHARM_CI_TCPSERVER "Build Charm with TCP command interface support" ON )
    OPTION( CHARM_CI_LOCALSERVER "Build Charm with local socket command interface support" ON )
ENDIF()

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
