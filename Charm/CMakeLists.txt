INCLUDE_DIRECTORIES( ${Charm_SOURCE_DIR} )

SET(
	CharmApplication_SRCS
	CharmWindow.cpp
	Application.cpp
	ViewHelpers.cpp
	TaskIdDialog.cpp
	Data.cpp
	ConfigurationDialog.cpp
	CharmPreferences.cpp
	TasksView.cpp
	TasksWindow.cpp
	TaskEditor.cpp
	GUIState.cpp
	DateEntrySyncer.cpp
	EventView.cpp
	EventWindow.cpp
	EventEditor.cpp
	EventEditorDelegate.cpp
	SelectTaskDialog.cpp
	ModelConnector.cpp
	ViewFilter.cpp
	TaskModelAdapter.cpp
	EventModelAdapter.cpp
	EventModelFilter.cpp
	TasksViewDelegate.cpp
	CharmAboutDialog.cpp
	TimeTrackingView/TimeTrackingView.cpp
	TimeTrackingView/TimeTrackingSummaryWidget.cpp
	TimeTrackingView/WeeklySummary.cpp
	TimeTrackingView/TimeTrackingTaskSelector.cpp
	IdleWidget.cpp
	Reports/ReportDialog.cpp
	Reports/ReportConfigurationPage.cpp
	Reports/CharmReport.cpp
	Reports/ActivityReport.cpp
	Reports/WeeklyTimeSheet3.cpp
	Reports/ReportPreviewWindow.cpp
	Commands/CommandRelayCommand.cpp
	Commands/CommandModifyEvent.cpp
	Commands/CommandDeleteEvent.cpp
	Commands/CommandSetAllTasks.cpp
	Commands/CommandAddTask.cpp
	Commands/CommandModifyTask.cpp
	Commands/CommandDeleteTask.cpp
	Commands/CommandMakeEvent.cpp
	Commands/CommandExportToXml.cpp
	Commands/CommandImportFromXml.cpp
	Commands/CommandMakeAndActivateEvent.cpp
	Idle/IdleDetector.cpp
	Idle/IdleCorrectionDialog.cpp
	Idle/X11IdleDetector.cpp
)

IF( APPLE )
	LIST( APPEND CharmApplication_SRCS MacApplication.cpp )
	EXEC_PROGRAM( sw_vers ARGS -buildVersion OUTPUT_VARIABLE DARWIN_BUILDVERSION )
	STRING( REGEX MATCH "^[0-9]+" DARWIN_VERSION ${DARWIN_BUILDVERSION} )
	ADD_DEFINITIONS( -DDARWIN_VERSION=${DARWIN_VERSION} )
	IF( QT_MAC_USE_COCOA )
		LIST(
			APPEND CharmApplication_SRCS
			Idle/MacCocoaIdleDetector.mm MacCocoaApplication.mm
		)
	SET( CMAKE_EXE_LINKER_FLAGS "-framework Foundation -framework AppKit" )
	ELSE()
		LIST(
			APPEND CharmApplication_SRCS
			Idle/MacCarbonIdleDetector.cpp MacCarbonApplication.cpp
		)
	ENDIF()
ENDIF()

IF ( WIN32 )
	LIST( APPEND CharmApplication_SRCS Idle/WindowsIdleDetector.cpp )
ENDIF()

CONFIGURE_FILE( config-charm.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-charm.h )

QT4_AUTOMOC( ${CharmApplication_SRCS} )

QT4_WRAP_UI(
	UiGenerated_SRCS
	MainWindow.ui
	ConfigurationDialog.ui
	CharmPreferences.ui
	TaskIdDialog.ui
	Reports/ReportDialog.ui
	Reports/ActivityReportConfigurationPage.ui
	Reports/WeeklyTimeSheetConfigurationPage.ui
	Reports/ReportPreviewWindow.ui
	TasksView.ui
	TaskEditor.ui
	EventView.ui
	EventEditor.ui
	SelectTaskDialog.ui
	CharmAboutDialog.ui
	TimeTrackingView/TimeTrackingView.ui
	Idle/IdleCorrectionDialog.ui
)

QT4_ADD_RESOURCES( Resources_SRCS CharmResources.qrc )

ADD_LIBRARY(
	CharmApplication STATIC
	${CharmApplication_SRCS} ${UiGenerated_SRCS}
)

SET( Charm_SRCS Charm.cpp )
QT4_AUTOMOC( ${Charm_SRCS} )

IF( APPLE )
	TARGET_LINK_LIBRARIES( CharmApplication IOKit )
	SET( MACOSX_BUNDLE_INFO_STRING "Charm ${Charm_VERSION}" )
	SET( MACOSX_BUNDLE_BUNDLE_VERSION "Charm ${Charm_VERSION}" )
	SET( MACOSX_BUNDLE_LONG_VERSION_STRING "Charm ${Charm_VERSION}" )
	SET( MACOSX_BUNDLE_SHORT_VERSION_STRING "${Charm_VERSION}" )
	SET( MACOSX_BUNDLE_COPYRIGHT "2006-2010 KDAB" )
	SET( MACOSX_BUNDLE_ICON_FILE "Charm.icns" )
	SET( MACOSX_BUNDLE_GUI_IDENTIFIER "com.kdab" )
	SET( MACOSX_BUNDLE_BUNDLE_NAME "Charm" )

	SET( RESOURCES "${Charm_BINARY_DIR}/Charm/Charm.app/Contents/Resources"	)
	SET( ICON "${Charm_SOURCE_DIR}/Charm/Icons/${MACOSX_BUNDLE_ICON_FILE}" )
	EXECUTE_PROCESS(
		COMMAND	${CMAKE_COMMAND} -E make_directory ${RESOURCES}
	)
	EXECUTE_PROCESS(
		COMMAND	${CMAKE_COMMAND} -E copy_if_different ${ICON} ${RESOURCES}
	)

	SET( CMAKE_MODULE_PATH ${Charm_SOURCE_DIR}/Charm/ ${CMAKE_MODULE_PATH} )
ENDIF()

IF( UNIX AND NOT APPLE )
	FIND_PACKAGE( X11 )
	IF( X11_FOUND )
		INCLUDE_DIRECTORIES( ${X11_INCLUDE_DIR} )
		IF( X11_Xscreensaver_LIB )
			MESSAGE( "X11 idle detection enabled." )
			TARGET_LINK_LIBRARIES( CharmApplication ${X11_Xscreensaver_LIB} )
		ELSE()
			MESSAGE( WARNING "Install libXss-dev for X11 idle detection." )
		ENDIF()
	ENDIF()
ENDIF()

IF( WIN32 )
	IF( MSVC )
		SET( Resources_SRCS ${Resources_SRCS} Charm.rc )
	ENDIF()
	TARGET_LINK_LIBRARIES( CharmApplication ${QT_QTMAIN_LIBRARY} )
	ADD_EXECUTABLE( Charm WIN32 ${Charm_SRCS} ${Resources_SRCS} )
ELSE()
	ADD_EXECUTABLE( Charm MACOSX_BUNDLE ${Charm_SRCS} ${Resources_SRCS} )
ENDIF()

TARGET_LINK_LIBRARIES( Charm CharmApplication )
TARGET_LINK_LIBRARIES( Charm CharmCore )
TARGET_LINK_LIBRARIES( Charm ${QT_LIBRARIES} )
MESSAGE( STATUS "Charm will be installed to ${CMAKE_INSTALL_PREFIX}" )

IF( UNIX AND NOT APPLE )
	SET( XDG_APPS_INSTALL_DIR share/applications )
	SET( ICON_INSTALL_DIR share/icons/hicolor/128x128/apps )
	INSTALL( FILES Charm.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
	INSTALL( FILES Icons/Charm-128x128.png DESTINATION ${ICON_INSTALL_DIR} )
ENDIF()

INSTALL( TARGETS Charm DESTINATION ${BIN_INSTALL_DIR} )

# Only support packaging on newer versions of CMake.
IF( NOT "${CMAKE_VERSION}" VERSION_LESS "2.8.0" )
	SET( QT_SQLDRIVERS_PLUGINS_DIR ${QT_PLUGINS_DIR}/sqldrivers )
	IF( WIN32 )
		FIND_FILE(
			QT_SQLITE_PLUGIN_LIBRARY_RELEASE
			NAMES qsqlite.dll qsqlite4.dll
			PATHS ${QT_SQLDRIVERS_PLUGINS_DIR}
		)
		FIND_FILE(
			QT_SQLITE_PLUGIN_LIBRARY_DEBUG
			NAMES qsqlite_debug.dll qsqlited.dll qsqlited4.dll
			PATHS ${QT_SQLDRIVERS_PLUGINS_DIR}
		)
	ELSE()
		FIND_LIBRARY(
			QT_SQLITE_PLUGIN_LIBRARY_RELEASE
			NAMES qsqlite qsqlite4
			PATHS ${QT_SQLDRIVERS_PLUGINS_DIR}
		)
		FIND_LIBRARY(
			QT_SQLITE_PLUGIN_LIBRARY_DEBUG
			NAMES qsqlite_debug qsqlited qsqlited4
			PATHS ${QT_SQLDRIVERS_PLUGINS_DIR}
		)
	ENDIF()
	INCLUDE( SelectLibraryConfigurations )
	SELECT_LIBRARY_CONFIGURATIONS( QT_SQLITE_PLUGIN )

	IF( QT_SQLITE_PLUGIN_LIBRARY_RELEASE OR QT_SQLITE_PLUGIN_LIBRARY_DEBUG )
	INSTALL(
		CODE "
			IF( \"\${CMAKE_INSTALL_CONFIG_NAME}\" MATCHES \"^([Dd][Ee][Bb][Uu][Gg])$\" )
				SET( QT_SQL_PLUGINS ${QT_SQLITE_PLUGIN_LIBRARY_DEBUG} )
			ELSE()
				SET( QT_SQL_PLUGINS ${QT_SQLITE_PLUGIN_LIBRARY_RELEASE} )
			ENDIF()
		"
	)
	ENDIF()

	SET( CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE )
	INCLUDE( InstallRequiredSystemLibraries )
	SET( CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_RELEASE ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} )
	SET( CMAKE_INSTALL_DEBUG_LIBRARIES TRUE )
	INCLUDE( InstallRequiredSystemLibraries )
	SET( CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_DEBUG ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} )

	IF( APPLE )
		SET( EXECUTABLE Charm.app )
		SET( PLUGINS_DIR_ONLY PlugIns )
		SET( PLUGINS_DIR ${EXECUTABLE}/Contents/${PLUGINS_DIR_ONLY} )
	ELSE()
		SET( EXECUTABLE Charm${CMAKE_EXECUTABLE_SUFFIX} )
		SET( PLUGINS_DIR plugins )
	ENDIF()

	IF( NOT BIN_INSTALL_DIR STREQUAL "." )
		SET( EXECUTABLE ${BIN_INSTALL_DIR}/${EXECUTABLE} )
	ENDIF()

	INSTALL(
		CODE "
			IF( \${CMAKE_INSTALL_PREFIX} MATCHES .*/_CPack_Packages/.* OR WIN32 )
				IF( \"\${CMAKE_INSTALL_CONFIG_NAME}\" MATCHES \"^([Dd][Ee][Bb][Uu][Gg])$\" )
					SET( CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS \"${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_DEBUG}\" )
				ELSE()
					SET( CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS \"${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_RELEASE}\" )
				ENDIF()
				FILE( INSTALL DESTINATION \"\${CMAKE_INSTALL_PREFIX}/\" TYPE FILES \${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} )

				FILE( INSTALL DESTINATION \"\${CMAKE_INSTALL_PREFIX}/${PLUGINS_DIR}/sqldrivers\" TYPE FILES \${QT_SQL_PLUGINS} )
				FILE( GLOB QT_SQL_PLUGINS \"\${CMAKE_INSTALL_PREFIX}/${PLUGINS_DIR}/sqldrivers/*\" )
				IF( APPLE )
					FILE( WRITE \"\${CMAKE_INSTALL_PREFIX}/Charm.app/Contents/Resources/qt.conf\" \"[Paths]\nPlugins = ${PLUGINS_DIR_ONLY}\" )
				ENDIF()

				INCLUDE( \"${CMAKE_SOURCE_DIR}/CMake/Modules/BundleUtilities.cmake\" )
				SET( BU_CHMOD_BUNDLE_ITEMS TRUE )
				FIXUP_BUNDLE( \"\${CMAKE_INSTALL_PREFIX}/${EXECUTABLE}\" \"\${QT_SQL_PLUGINS}\" \"${QT_LIBRARY_DIR}\" )
			ENDIF()
		"
	)
ENDIF()
