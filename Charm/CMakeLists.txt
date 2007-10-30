IF ( QT4_FOUND )
   INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} )
   INCLUDE_DIRECTORIES( ${QT_QTCORE_INCLUDE_DIR} )
   INCLUDE_DIRECTORIES( ${QT_QTGUI_INCLUDE_DIR} )
   INCLUDE_DIRECTORIES( ${QT_QTSQL_INCLUDE_DIR} )
   INCLUDE_DIRECTORIES( ${QT_QTXML_INCLUDE_DIR} )
   INCLUDE_DIRECTORIES( ${QT_QTOPENGL_INCLUDE_DIR} )
   INCLUDE_DIRECTORIES( .. )

   LINK_LIBRARIES( ${QT_QTCORE_LIBRARY} )
   LINK_LIBRARIES( ${QT_QTOPENGL_LIBRARY} )
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
		       ${UiGenerated_SRCS} 
		       ${Resources_SRCS} )

    SET( Charm_SRCS Charm.cpp )
    QT4_AUTOMOC (${Charm_SRCS} )

    ADD_EXECUTABLE( Charm MACOSX_BUNDLE ${Charm_SRCS} )
    TARGET_LINK_LIBRARIES( Charm CharmApplication )
    TARGET_LINK_LIBRARIES( Charm CharmCore )
    INSTALL( TARGETS Charm DESTINATION bin )

ENDIF ( QT4_FOUND )

IF ( NOT QT4_FOUND )
   MESSAGE( "Fatal: Qt 4 not found." )
ENDIF ( NOT QT4_FOUND) 

