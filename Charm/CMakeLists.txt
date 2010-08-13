IF( QT4_FOUND )

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
      SET( MACOSX_BUNDLE_INFO_STRING "Charm - Version ${Charm_VERSION}" )
      SET( MACOSX_BUNDLE_BUNDLE_VERSION "${Charm_VERSION}" )
      SET( MACOSX_BUNDLE_ICON_FILE "Charm.icns" )
      SET( MACOSX_BUNDLE_GUI_IDENTIFIER "net.kdab" )
      SET( MACOSX_BUNDLE_BUNDLE_NAME "Charm" )

      EXEC_PROGRAM( "mkdir -p ${Charm_BINARY_DIR}/Charm/Charm.app/Contents/Resources" )
      EXEC_PROGRAM( "cp ${Charm_SOURCE_DIR}/Charm/Icons/Charm.icns ${Charm_BINARY_DIR}/Charm/Charm.app/Contents/Resources/" )

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
   IF( CHARM_BUILD_STATIC_QT )
      FIND_LIBRARY( QT_JPEG_PLUGINLIBRARY NAMES qjpeg qjpeg_debug PATHS ${QT_PLUGINS_DIR}/imageformats )
      TARGET_LINK_LIBRARIES( Charm ${QT_JPEG_PLUGINLIBRARY} )
      MESSAGE( "Linking static Qt JPG plugin for this static build." )
   ENDIF( CHARM_BUILD_STATIC_QT )
   MESSAGE( "Charm will be installed to ${CMAKE_INSTALL_PREFIX}" )

   IF( UNIX AND NOT APPLE )
      SET( XDG_APPS_INSTALL_DIR share/applications )
      SET( ICON_INSTALL_DIR share/icons/hicolor/128x128/apps )
      INSTALL( FILES Charm.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
      INSTALL( FILES Icons/Charm-128x128.png DESTINATION ${ICON_INSTALL_DIR} )
      INSTALL( TARGETS Charm DESTINATION bin )
   ELSE( UNIX AND NOT APPLE )
      INSTALL( TARGETS Charm DESTINATION . )
   ENDIF( UNIX AND NOT APPLE )

   IF ( WIN32 )
      SET( CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE )
      INCLUDE( InstallRequiredSystemLibraries )
      INSTALL( FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION . )

      INSTALL( CODE "

   IF( \${CMAKE_INSTALL_PREFIX} MATCHES .*/_CPack_Packages/.* )
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
      INCLUDE( GetPrerequisites )
      INCLUDE( \"${CMAKE_SOURCE_DIR}/CMake/Modules/BundleUtilities.cmake\" )
      SET( chmod_bundle_items TRUE )
      FIXUP_BUNDLE( \"\${CMAKE_INSTALL_PREFIX}/Charm.app\" \"\" \"\" )
   ENDIF( \${CMAKE_INSTALL_PREFIX} MATCHES .*/_CPack_Packages/.* )

      " )
   ENDIF ( APPLE )

ELSE( QT4_FOUND )
   MESSAGE( "Fatal: Qt 4 not found." )
ENDIF( QT4_FOUND )
