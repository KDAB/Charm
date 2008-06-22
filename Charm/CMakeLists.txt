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
   
   IF (CMAKE_COMPILER_IS_GNUCXX)
      ADD_DEFINITIONS( -Wall -Wundef -Wcast-align -Wconversion -Wchar-subscripts -Wpointer-arith 
		       -Wwrite-strings -Wpacked -Wformat-security -Wmissing-format-attribute 
		       -Wold-style-cast )
# to be added:       
#	-Woverloaded-virtual  -Wshadow 
#  -Wnon-virtual-dtor # Qt has too many of those already
   ENDIF (CMAKE_COMPILER_IS_GNUCXX)

   SET( CharmApplication_SRCS 
		       MainWindow.cpp
		       Application.cpp
		       ViewHelpers.cpp
		       TaskIdDialog.cpp
		       Data.cpp
		       ConfigurationDialog.cpp 
		       CharmPreferences.cpp
		       TasksView.cpp
		       GUIState.cpp
		       EventEditor.cpp 
		       EventEditorDelegate.cpp 
		       SelectTaskDialog.cpp
		       ModelConnector.cpp
		       ViewFilter.cpp
		       TaskModelAdapter.cpp
		       EventModelAdapter.cpp
		       EventModelFilter.cpp
		       ChattyItemDelegate.cpp 
		       CharmAboutDialog.cpp
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
		       Commands/CommandAddTask.cpp
		       Commands/CommandModifyTask.cpp
		       Commands/CommandDeleteTask.cpp
		       Commands/CommandMakeEvent.cpp
		       Commands/CommandExportToXml.cpp
		       Commands/CommandImportFromXml.cpp
		       Commands/CommandMakeAndActivateEvent.cpp 
)
    
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
		       EventEditor.ui 
		       SelectTaskDialog.ui
		       CharmAboutDialog.ui )

    QT4_ADD_RESOURCES ( Resources_SRCS CharmResources.qrc )

    ADD_LIBRARY( CharmApplication STATIC 
		       ${CharmApplication_SRCS}
		       ${UiGenerated_SRCS} )

    SET( Charm_SRCS Charm.cpp )
    QT4_AUTOMOC (${Charm_SRCS} )

	IF( APPLE )
		SET( MACOSX_BUNDLE_INFO_STRING "Charm - Version 1.1.0-trunk" )
		SET( MACOSX_BUNDLE_BUNDLE_VERSION "1.1.0" )
		SET( MACOSX_BUNDLE_ICON_FILE "Charm.icns" )
		SET( MACOSX_BUNDLE_GUI_IDENTIFIER "net.kdab" )
		SET( MACOSX_BUNDLE_BUNDLE_NAME "Charm" )
		
		EXEC_PROGRAM( "mkdir -p ${Charm_BINARY_DIR}/Charm/Charm.app/Contents/Resources")
		EXEC_PROGRAM( "cp ${Charm_SOURCE_DIR}/Charm/Icons/Charm.icns ${Charm_BINARY_DIR}/Charm/Charm.app/Contents/Resources/")
		
		SET( CMAKE_MODULE_PATH ${Charm_SOURCE_DIR}/Charm/ ${CMAKE_MODULE_PATH} )
	ENDIF( APPLE)

    ADD_EXECUTABLE( Charm MACOSX_BUNDLE ${Charm_SRCS} ${Resources_SRCS} )
    TARGET_LINK_LIBRARIES( Charm CharmApplication )
    TARGET_LINK_LIBRARIES( Charm CharmCore )
    TARGET_LINK_LIBRARIES( Charm ${QT_LIBRARIES} )
    IF( CHARM_BUILD_STATIC_QT )
    	FIND_LIBRARY( QT_JPEG_PLUGINLIBRARY NAMES qjpeg qjpeg_debug PATHS ${QT_PLUGINS_DIR}/imageformats )
	    TARGET_LINK_LIBRARIES( Charm ${QT_JPEG_PLUGINLIBRARY} )
    	MESSAGE( "Linking static Qt JPG plugin for this static build." )
    ENDIF( CHARM_BUILD_STATIC_QT )
    IF(APPLE)
        INSTALL( TARGETS Charm DESTINATION /Applications )
    ELSE(APPLE)
	INSTALL( TARGETS Charm DESTINATION bin )
    ENDIF(APPLE)

ENDIF ( QT4_FOUND )

IF ( NOT QT4_FOUND )
   MESSAGE( "Fatal: Qt 4 not found." )
ENDIF ( NOT QT4_FOUND) 

