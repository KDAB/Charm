IF( QT4_FOUND )
   ADD_DEFINITIONS( -DCHARM_VERSION=\"${Charm_VERSION}\" )

   INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} )
   INCLUDE_DIRECTORIES( ${QT_QTCORE_INCLUDE_DIR} )
   INCLUDE_DIRECTORIES( ${QT_QTGUI_INCLUDE_DIR} )
   INCLUDE_DIRECTORIES( ${QT_QTSQL_INCLUDE_DIR} )
   INCLUDE_DIRECTORIES( ${QT_QTXML_INCLUDE_DIR} )
   INCLUDE_DIRECTORIES( .. )
   INCLUDE( ${QT_USE_FILE} )

   SET( CharmApplication_SRCS 
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
        EventView.cpp
        EventWindow.cpp
        EventEditor.cpp
        EventDisplay.cpp
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
        # OpeningScene.cpp
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
   SET( HAVE_LIBXSS 0 )
   IF( APPLE )
      SET( CharmApplication_SRCS ${CharmApplication_SRCS} MacApplication.cpp )
      IF( QT_MAC_USE_COCOA )
         SET( CharmApplication_SRCS
              ${CharmApplication_SRCS}
              Idle/MacCocoaIdleDetector.mm
              MacCocoaApplication.mm )
         SET( CMAKE_EXE_LINKER_FLAGS "-framework Foundation -framework AppKit" )
      ELSE( QT_MAC_USE_COCOA )
         SET( CharmApplication_SRCS
              ${CharmApplication_SRCS}
              Idle/MacCarbonIdleDetector.cpp
              MacCarbonApplication.cpp )
      ENDIF( QT_MAC_USE_COCOA )
      EXEC_PROGRAM( sw_vers ARGS -buildVersion OUTPUT_VARIABLE DARWIN_BUILDVERSION )
      STRING( REGEX MATCH "^[0-9]+" DARWIN_VERSION ${DARWIN_BUILDVERSION} )
      ADD_DEFINITIONS( -DDARWIN_VERSION=${DARWIN_VERSION} )
   ELSE( APPLE )
      IF( UNIX )
         INCLUDE( FindX11 )
         IF( X11_FOUND )
            INCLUDE_DIRECTORIES( ${X11_INCLUDE_DIR} )
            IF( X11_Xscreensaver_LIB )
               MESSAGE( "Found screensaver extension library, X11 idle detection enabled" )
               LINK_LIBRARIES( ${X11_Xscreensaver_LIB} )
               SET( HAVE_LIBXSS 1 )
            ELSE( X11_Xscreensaver_LIB )
               MESSAGE( "Please install libXss-dev to get idle detection to work on Linux" )
            ENDIF( X11_Xscreensaver_LIB )
         ENDIF( X11_FOUND )
      ENDIF( UNIX )
   ENDIF( APPLE )
   IF ( WIN32 )
   SET( CharmApplication_SRCS
        ${CharmApplication_SRCS}
        Idle/WindowsIdleDetector.cpp )
   ENDIF( WIN32 )

  CONFIGURE_FILE( config-charm.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-charm.h )

   QT4_AUTOMOC( ${CharmApplication_SRCS} )

   QT4_WRAP_UI( UiGenerated_SRCS
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
             EventDisplay.ui
             EventEditor.ui
             SelectTaskDialog.ui
             CharmAboutDialog.ui
             TimeTrackingView/TimeTrackingView.ui
             Idle/IdleCorrectionDialog.ui )

   QT4_ADD_RESOURCES ( Resources_SRCS CharmResources.qrc )

   ADD_LIBRARY( CharmApplication STATIC
                ${CharmApplication_SRCS}
                ${UiGenerated_SRCS} )

   SET( Charm_SRCS Charm.cpp )
   QT4_AUTOMOC (${Charm_SRCS} )

   IF( APPLE )
      TARGET_LINK_LIBRARIES( CharmApplication IOKit )
      SET( MACOSX_BUNDLE_INFO_STRING "Charm ${Charm_VERSION}" )
      SET( MACOSX_BUNDLE_BUNDLE_VERSION "Charm ${Charm_VERSION}" )
      SET( MACOSX_BUNDLE_LONG_VERSION_STRING "Charm ${Charm_VERSION}" )
      SET( MACOSX_BUNDLE_SHORT_VERSION_STRING "${Charm_VERSION}" )
      SET( MACOSX_BUNDLE_COPYRIGHT "2006-2010 Mirko Boehm" )
      SET( MACOSX_BUNDLE_ICON_FILE "Charm.icns" )
      SET( MACOSX_BUNDLE_GUI_IDENTIFIER "com.kdab" )
      SET( MACOSX_BUNDLE_BUNDLE_NAME "Charm" )

      EXECUTE_PROCESS( COMMAND "${CMAKE_COMMAND} -E make_directory ${Charm_BINARY_DIR}/Charm/Charm.app/Contents/Resources"
                       COMMAND "${CMAKE_COMMAND} -E copy_if_different ${Charm_SOURCE_DIR}/Charm/Icons/Charm.icns ${Charm_BINARY_DIR}/Charm/Charm.app/Contents/Resources/" )

      SET( CMAKE_MODULE_PATH ${Charm_SOURCE_DIR}/Charm/ ${CMAKE_MODULE_PATH} )
   ENDIF( APPLE )

   IF( WIN32 )
      IF( MSVC )
         SET( Resources_SRCS ${Resources_SRCS} Charm.rc )
      ENDIF( MSVC )
      LINK_LIBRARIES( ${QT_QTMAIN_LIBRARY} )
      ADD_EXECUTABLE( Charm WIN32 ${Charm_SRCS} ${Resources_SRCS} )
   ELSE( WIN32 )
      ADD_EXECUTABLE( Charm MACOSX_BUNDLE ${Charm_SRCS} ${Resources_SRCS} )
   ENDIF( WIN32 )

   TARGET_LINK_LIBRARIES( Charm CharmApplication )
   TARGET_LINK_LIBRARIES( Charm CharmCore )
   TARGET_LINK_LIBRARIES( Charm ${QT_LIBRARIES} )
   MESSAGE( "Charm will be installed to ${CMAKE_INSTALL_PREFIX}" )

   IF( UNIX AND NOT APPLE )
      SET( BIN_INSTALL_DIR bin )
      SET( XDG_APPS_INSTALL_DIR share/applications )
      SET( ICON_INSTALL_DIR share/icons/hicolor/128x128/apps )
      INSTALL( FILES Charm.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
      INSTALL( FILES Icons/Charm-128x128.png DESTINATION ${ICON_INSTALL_DIR} )
      INSTALL( TARGETS Charm DESTINATION bin )
   ELSE( UNIX AND NOT APPLE )
      SET( BIN_INSTALL_DIR . )
   ENDIF( UNIX AND NOT APPLE )

   INSTALL( TARGETS Charm DESTINATION ${BIN_INSTALL_DIR} )

   IF( WIN32 )
      FIND_FILE( QT_SQLITE_PLUGIN_LIBRARY_RELEASE NAMES qsqlite.dll qsqlite4.dll PATHS ${QT_PLUGINS_DIR}/sqldrivers )
      FIND_FILE( QT_SQLITE_PLUGIN_LIBRARY_DEBUG NAMES qsqlite_debug.dll qsqlited.dll qsqlited4.dll PATHS ${QT_PLUGINS_DIR}/sqldrivers )
   ELSE( WIN32 )
      FIND_LIBRARY( QT_SQLITE_PLUGIN_LIBRARY_RELEASE NAMES qsqlite qsqlite4 PATHS ${QT_PLUGINS_DIR}/sqldrivers )
      FIND_LIBRARY( QT_SQLITE_PLUGIN_LIBRARY_DEBUG NAMES qsqlite_debug qsqlited qsqlited4 PATHS ${QT_PLUGINS_DIR}/sqldrivers )
   ENDIF( WIN32 )
   INCLUDE( SelectLibraryConfigurations )
   SELECT_LIBRARY_CONFIGURATIONS( QT_SQLITE_PLUGIN )


   IF( QT_SQLITE_PLUGIN_LIBRARY_RELEASE OR QT_SQLITE_PLUGIN_LIBRARY_DEBUG )
   INSTALL( CODE "

      IF( \"\${CMAKE_INSTALL_CONFIG_NAME}\" MATCHES \"^([Dd][Ee][Bb][Uu][Gg])$\" )
         SET( QT_SQL_PLUGINS ${QT_SQLITE_PLUGIN_LIBRARY_DEBUG} )
      ELSE( \"\${CMAKE_INSTALL_CONFIG_NAME}\" MATCHES \"^([Dd][Ee][Bb][Uu][Gg])$\" )
         SET( QT_SQL_PLUGINS ${QT_SQLITE_PLUGIN_LIBRARY_RELEASE} )
      ENDIF( \"\${CMAKE_INSTALL_CONFIG_NAME}\" MATCHES \"^([Dd][Ee][Bb][Uu][Gg])$\" )

   " )
   ENDIF( QT_SQLITE_PLUGIN_LIBRARY_RELEASE OR QT_SQLITE_PLUGIN_LIBRARY_DEBUG )

   IF ( WIN32 )
      SET( CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE )
      INCLUDE( InstallRequiredSystemLibraries )
      SET( MSVC_RUNTIME_LIBRARIES_RELEASE ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} )
      SET( CMAKE_INSTALL_DEBUG_LIBRARIES TRUE )
      INCLUDE( InstallRequiredSystemLibraries )
      SET( MSVC_RUNTIME_LIBRARIES_DEBUG ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} )

      INSTALL( CODE "

   IF( \${CMAKE_INSTALL_PREFIX} MATCHES .*/_CPack_Packages/.* )
      IF( \"\${CMAKE_INSTALL_CONFIG_NAME}\" MATCHES \"^([Dd][Ee][Bb][Uu][Gg])$\" )
         SET( MSVC_RUNTIME \"${MSVC_RUNTIME_LIBRARIES_DEBUG}\" )
      ELSE( \"\${CMAKE_INSTALL_CONFIG_NAME}\" MATCHES \"^([Dd][Ee][Bb][Uu][Gg])$\" )
         SET( MSVC_RUNTIME \"${MSVC_RUNTIME_LIBRARIES_RELEASE}\" )
      ENDIF( \"\${CMAKE_INSTALL_CONFIG_NAME}\" MATCHES \"^([Dd][Ee][Bb][Uu][Gg])$\" )
      FILE( INSTALL DESTINATION \"\${CMAKE_INSTALL_PREFIX}/\" TYPE FILES \${MSVC_RUNTIME} )

      IF( QT_SQL_PLUGINS )
         FILE( INSTALL DESTINATION \"\${CMAKE_INSTALL_PREFIX}/PlugIns/sqldrivers\" TYPE FILES \${QT_SQL_PLUGINS} )
      ENDIF( QT_SQL_PLUGINS )

      INCLUDE( GetPrerequisites )
      SET( EXECUTABLE \"\${CMAKE_INSTALL_PREFIX}/Charm${CMAKE_EXECUTABLE_SUFFIX}\" )
      GET_PREREQUISITES( \"\${EXECUTABLE}\" PREREQUISITES 1 1 \"\${EXECUTABLE}\" \"${QT_BINARY_DIR};${QT_LIBRARY_DIR}\" )
      FOREACH( PREREQUISITE \${PREREQUISITES} )
         GP_RESOLVE_ITEM( \"\" \"\${PREREQUISITE}\" \"\" \"\" RESOLVED )
         FILE( INSTALL DESTINATION \"\${CMAKE_INSTALL_PREFIX}/\" TYPE FILES \"\${RESOLVED}\" )
      ENDFOREACH( PREREQUISITE \${PREREQUISITES} )
   ENDIF( \${CMAKE_INSTALL_PREFIX} MATCHES .*/_CPack_Packages/.* )

      " )
   ENDIF ( WIN32 )

   IF ( APPLE )
      INSTALL( CODE "

   IF( \${CMAKE_INSTALL_PREFIX} MATCHES .*/_CPack_Packages/.* )
      FILE( INSTALL DESTINATION \"\${CMAKE_INSTALL_PREFIX}/Charm.app/Contents/PlugIns/sqldrivers\" TYPE FILES \${QT_SQL_PLUGINS} )
      FILE( GLOB QT_SQL_PLUGINS \"\${CMAKE_INSTALL_PREFIX}/Charm.app/Contents/PlugIns/sqldrivers/*\" )
      FILE( WRITE \"\${CMAKE_INSTALL_PREFIX}/Charm.app/Contents/Resources/qt.conf\" \"[Paths]\nPlugins = PlugIns\" )

      INCLUDE( GetPrerequisites )
      INCLUDE( \"${CMAKE_SOURCE_DIR}/CMake/Modules/BundleUtilities.cmake\" )
      SET( chmod_bundle_items TRUE )
      FIXUP_BUNDLE( \"\${CMAKE_INSTALL_PREFIX}/Charm.app\" \"\${QT_SQL_PLUGINS}\" \"\" )
   ENDIF( \${CMAKE_INSTALL_PREFIX} MATCHES .*/_CPack_Packages/.* )

      " )
   ENDIF ( APPLE )

ELSE( QT4_FOUND )
   MESSAGE( "Fatal: Qt 4 not found." )
ENDIF( QT4_FOUND )
