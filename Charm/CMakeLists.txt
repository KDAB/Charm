INCLUDE_DIRECTORIES( ${Charm_SOURCE_DIR} ${Charm_BINARY_DIR} )

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
        TimeTrackingView/TimeTrackingWindow.cpp
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
)

SET(IDLEDETECTION_LIBS)
IF( CHARM_IDLE_DETECTION )
    IF( APPLE )
            IF( QT_MAC_USE_COCOA )
                    LIST( APPEND CharmApplication_SRCS Idle/MacCocoaIdleDetector.mm )
            ELSE()
                    LIST( APPEND CharmApplication_SRCS Idle/MacCarbonIdleDetector.cpp )
            ENDIF()
    ELSEIF( WIN32 )
            LIST( APPEND CharmApplication_SRCS Idle/WindowsIdleDetector.cpp )
    ELSEIF( UNIX )
            FIND_PACKAGE( X11 )
            IF( X11_FOUND AND X11_Xscreensaver_LIB )
                    MESSAGE( "X11 idle detection enabled." )
                    INCLUDE_DIRECTORIES( ${X11_INCLUDE_DIR} )
                    LIST( APPEND CharmApplication_SRCS Idle/X11IdleDetector.cpp )
                    SET(IDLEDETECTION_LIBS ${X11_X11_LIB} ${X11_Xscreensaver_LIB})
            ELSE()
                    MESSAGE( WARNING "Install libXss-dev for X11 idle detection." )
            ENDIF()
    ENDIF()
ENDIF()

IF( APPLE )
        LIST( APPEND CharmApplication_SRCS MacApplication.cpp )
        IF( QT_MAC_USE_COCOA )
                LIST( APPEND CharmApplication_SRCS MacCocoaApplication.mm )
                SET( CMAKE_EXE_LINKER_FLAGS "-framework AppKit" )
        ELSE()
                LIST( APPEND CharmApplication_SRCS MacCarbonApplication.cpp )
                SET( CMAKE_EXE_LINKER_FLAGS "-framework IOKit" )
        ENDIF()
ENDIF()

QT4_AUTOMOC( ${CharmApplication_SRCS} )

QT4_WRAP_UI(
        UiGenerated_SRCS
        ConfigurationDialog.ui
        CharmPreferences.ui
        TaskIdDialog.ui
        Reports/ReportDialog.ui
        Reports/ActivityReportConfigurationPage.ui
        Reports/WeeklyTimeSheetConfigurationPage.ui
        Reports/ReportPreviewWindow.ui
        TaskEditor.ui
        EventEditor.ui
        SelectTaskDialog.ui
        CharmAboutDialog.ui
        TimeTrackingView/TimeTrackingWindow.ui
        Idle/IdleCorrectionDialog.ui
)

QT4_ADD_RESOURCES( Resources_SRCS CharmResources.qrc )

ADD_LIBRARY(
        CharmApplication STATIC
        ${CharmApplication_SRCS} ${UiGenerated_SRCS}
)
IF( IDLEDETECTION_LIBS )
  TARGET_LINK_LIBRARIES(CharmApplication ${IDLEDETECTION_LIBS})
ENDIF( IDLEDETECTION_LIBS )

SET( Charm_SRCS Charm.cpp )
QT4_AUTOMOC( ${Charm_SRCS} )

IF( APPLE )
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

ENDIF()

IF( MSVC )
        SET( Resources_SRCS ${Resources_SRCS} Charm.rc )
ENDIF()

ADD_EXECUTABLE( Charm WIN32 MACOSX_BUNDLE ${Charm_SRCS} ${Resources_SRCS} )
TARGET_LINK_LIBRARIES( Charm CharmApplication CharmCore ${QT_LIBRARIES} )
MESSAGE( STATUS "Charm will be installed to ${CMAKE_INSTALL_PREFIX}" )

IF( UNIX AND NOT APPLE )
        SET( XDG_APPS_INSTALL_DIR share/applications )
        INSTALL( FILES Charm.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
        INSTALL( FILES Icons/Charm-128x128.png DESTINATION share/icons/hicolor/128x128/apps )
        INSTALL( FILES Icons/charmtray16.png DESTINATION share/icons/hicolor/16x16/apps )
        INSTALL( FILES Icons/charmtray24.png DESTINATION share/icons/hicolor/24x24/apps )
        INSTALL( FILES Icons/01.png Icons/02.png Icons/03.png Icons/04.png
	               Icons/05.png Icons/06.png Icons/07.png Icons/08.png
                       Icons/09.png Icons/10.png Icons/11.png Icons/12.png
                 DESTINATION  share/icons/hicolor/24x24/apps )
ENDIF()

INSTALL( TARGETS Charm DESTINATION ${BIN_INSTALL_DIR} )

# Only support packaging on newer versions of CMake.
IF( NOT "${CMAKE_VERSION}" VERSION_LESS "2.8.3" )
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

                                INCLUDE( \"${CMAKE_ROOT}/Modules/BundleUtilities.cmake\" )
                                SET( BU_CHMOD_BUNDLE_ITEMS TRUE )
                                FIXUP_BUNDLE( \"\${CMAKE_INSTALL_PREFIX}/${EXECUTABLE}\" \"\${QT_SQL_PLUGINS}\" \"${QT_LIBRARY_DIR}\" )
                        ENDIF()
                "
        )
ENDIF()
