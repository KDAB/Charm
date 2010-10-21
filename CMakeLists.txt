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
	SET( CMAKE_BUILD_TYPE "Release" )
ENDIF()

MESSAGE( STATUS "Building Charm ${Charm_VERSION} in ${CMAKE_BUILD_TYPE} mode" )

CMAKE_MINIMUM_REQUIRED( VERSION 2.6.0 )
FIND_PACKAGE( Qt4 4.6.0 COMPONENTS QtCore QtGui QtSql QtXml REQUIRED )
INCLUDE( UseQt4 )
ENABLE_TESTING()

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

ADD_DEFINITIONS( -DCHARM_VERSION=\"${Charm_VERSION}\" )

IF( UNIX AND NOT APPLE )
	SET( BIN_INSTALL_DIR bin )
	SET( DOC_INSTALL_DIR share/doc/charm/ )
ELSE()
	SET( BIN_INSTALL_DIR . )
	SET( DOC_INSTALL_DIR . )
ENDIF()

ADD_SUBDIRECTORY( Core )
ADD_SUBDIRECTORY( Charm )

IF( UNIX )
	OPTION( CHARM_ENABLE_TOOLS_BUILD "Build the Charm timesheet tools" OFF )
ENDIF()

IF( UNIX AND CHARM_ENABLE_TOOLS_BUILD )
	# Only build the tools if they are explicitly requested to avoid
	# the Qt MySQL driver dependency.
	ADD_SUBDIRECTORY( Tools/TimesheetProcessor )
	ADD_SUBDIRECTORY( Tools/TimesheetGenerator )
	MESSAGE( STATUS "Building the Charm timesheet tools")
ENDIF()

ADD_SUBDIRECTORY( Tests )

INSTALL( FILES ReadMe.txt License.txt DESTINATION ${DOC_INSTALL_DIR} )

# Only support packaging on newer versions of CMake.
IF( NOT "${CMAKE_VERSION}" VERSION_LESS "2.8.0" )
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
	    SET(
			CPACK_NSIS_MUI_ICON
			"${CMAKE_SOURCE_DIR}/Charm/Icons/Charm.ico"
		)
	    SET(
			CPACK_NSIS_MUI_UNIICON
			"${CMAKE_SOURCE_DIR}/Charm/Icons/NSIS-Uninstall.ico"
		)
	    SET( CPACK_NSIS_URL_INFO_ABOUT "http://www.kdab.com/" )
	    SET( CPACK_NSIS_INSTALLED_ICON_NAME "Charm.exe" )
	    SET( CPACK_NSIS_MENU_LINKS "Charm.exe" "Charm" )
	    SET(
			CPACK_NSIS_DEFINES
			"!define MUI_FINISHPAGE_RUN \\\"$INSTDIR\\\\Charm.exe\\\" "
		)
	ENDIF()
	IF( APPLE )
	    SET( CPACK_GENERATOR "DragNDrop" "ZIP" )
	    SET( CPACK_DMG_FORMAT "UDBZ" )
	    SET( CPACK_DMG_VOLUME_NAME "Charm" )
	    SET( CPACK_SYSTEM_NAME "OSX" )
	    SET( CPACK_PACKAGE_FILE_NAME "Charm-${Charm_VERSION}" )
	    SET(
			CPACK_PACKAGE_ICON
			"${CMAKE_SOURCE_DIR}/Charm/Icons/CharmDMG.icns"
		)
	ENDIF()

	INCLUDE( CPack )
ENDIF()