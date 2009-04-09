IF ( QT4_FOUND )

   INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} )
   INCLUDE_DIRECTORIES( ${QT_QTCORE_INCLUDE_DIR} )
   INCLUDE_DIRECTORIES( ${QT_QTGUI_INCLUDE_DIR} )
   INCLUDE_DIRECTORIES( ${QT_QTSQL_INCLUDE_DIR} )
   INCLUDE_DIRECTORIES( ${QT_QTXML_INCLUDE_DIR} )
   INCLUDE_DIRECTORIES( .. )
   include (${QT_USE_FILE})

   LINK_LIBRARIES( ${QT_QTCORE_LIBRARY} )
   LINK_LIBRARIES( ${QT_QTGUI_LIBRARY} )
   LINK_LIBRARIES( ${QT_QTXML_LIBRARY} )
   
   SET( CharmApplication_SRCS 
		       CharmWindow.cpp
		       Application.cpp
		       SpecialKeysEventFilter.cpp
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
		       TimeTrackingView.cpp
		       TimeTrackingSummaryWidget.cpp
		       # OpeningScene.cpp
		       IdleWidget.cpp
		       Reports/ReportDialog.cpp
		       Reports/ReportConfigurationPage.cpp
		       Reports/CharmReport.cpp
		       Reports/ActivityReport.cpp 
		       Reports/WeeklyTimeSheet3.cpp
		       Reports/ReportPreviewWindow.cpp 
		       Reports/ParagraphFormatCollection.cpp
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
	set(HAVE_LIBXSS 0)
	IF( APPLE )
	   SET( CharmApplication_SRCS 
		      ${CharmApplication_SRCS} Idle/MacOsIdleDetector.cpp
              mac_application.cpp )
	   EXEC_PROGRAM( sw_vers ARGS -buildVersion OUTPUT_VARIABLE DARWIN_BUILDVERSION )
	   STRING(REGEX MATCH "^[0-9]+" DARWIN_VERSION ${DARWIN_BUILDVERSION})
#	   MESSAGE(STATUS "DARWIN_VERSION=${DARWIN_VERSION}")
	   ADD_DEFINITIONS( -DDARWIN_VERSION=${DARWIN_VERSION} )
        ELSE( APPLE )
	    IF( UNIX )
                include(FindX11)
	        if(X11_FOUND)
	            if(X11_Xscreensaver_LIB)
	                message("Found screensaver extension library, X11 idle detection enabled")
	                LINK_LIBRARIES (${X11_Xscreensaver_LIB} )
	                set(HAVE_LIBXSS 1)
                    else(X11_Xscreensaver_LIB)
                        message("Please install libXss-dev to get idle detection to work on Linux")
                    endif(X11_Xscreensaver_LIB)
                endif(X11_FOUND)
            ENDIF( UNIX )
        ENDIF( APPLE)
        configure_file(config-charm.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-charm.h )


    
    QT4_AUTOMOC ( ${CharmApplication_SRCS} )

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
		       TimeTrackingView.ui
		       Idle/IdleCorrectionDialog.ui )

    QT4_ADD_RESOURCES ( Resources_SRCS CharmResources.qrc )

    ADD_LIBRARY( CharmApplication STATIC 
		       ${CharmApplication_SRCS}
		       ${UiGenerated_SRCS} )

    SET( Charm_SRCS Charm.cpp )
    QT4_AUTOMOC (${Charm_SRCS} )

	IF( APPLE )
	        TARGET_LINK_LIBRARIES( CharmApplication IOKit )
		SET( MACOSX_BUNDLE_INFO_STRING "Charm - Version 1.3-trunk" )
		SET( MACOSX_BUNDLE_BUNDLE_VERSION "1.3-trunk" )
		SET( MACOSX_BUNDLE_ICON_FILE "Charm.icns" )
		SET( MACOSX_BUNDLE_GUI_IDENTIFIER "net.kdab" )
		SET( MACOSX_BUNDLE_BUNDLE_NAME "Charm" )
		
		EXEC_PROGRAM( "mkdir -p ${Charm_BINARY_DIR}/Charm/Charm.app/Contents/Resources")
		EXEC_PROGRAM( "cp ${Charm_SOURCE_DIR}/Charm/Icons/Charm.icns ${Charm_BINARY_DIR}/Charm/Charm.app/Contents/Resources/")
		
		SET( CMAKE_MODULE_PATH ${Charm_SOURCE_DIR}/Charm/ ${CMAKE_MODULE_PATH} )
	ENDIF( APPLE)

    IF( WIN32 )
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

    IF( UNIX )
        SET( XDG_APPS_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/share/applications )
        SET( ICON_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/share/icons )
        INSTALL( FILES Charm.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
        INSTALL( FILES Icons/Charm-128x128.png DESTINATION ${ICON_INSTALL_DIR} )
        INSTALL( TARGETS Charm DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
    ELSE( UNIX )
        INSTALL( TARGETS Charm DESTINATION ${CMAKE_INSTALL_PREFIX} )
    ENDIF( UNIX )

ENDIF ( QT4_FOUND )

IF ( NOT QT4_FOUND )
   MESSAGE( "Fatal: Qt 4 not found." )
ENDIF ( NOT QT4_FOUND) 

