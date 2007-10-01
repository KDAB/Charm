IF ( QT4_FOUND )
   INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} )
   INCLUDE_DIRECTORIES( ${QT_QTCORE_INCLUDE_DIR} )
   INCLUDE_DIRECTORIES( ${QT_QTGUI_INCLUDE_DIR} )
   INCLUDE_DIRECTORIES( ${QT_QTSQL_INCLUDE_DIR} )
   INCLUDE_DIRECTORIES( ${QT_QTXML_INCLUDE_DIR} )

   LINK_LIBRARIES( ${QT_QTCORE_LIBRARY} )
   LINK_LIBRARIES( ${QT_QTGUI_LIBRARY} )
   LINK_LIBRARIES( ${QT_QTSQL_LIBRARY} )
   LINK_LIBRARIES( ${QT_QTXML_LIBRARY} )
   
   IF (CMAKE_COMPILER_IS_GNUCXX)
      ADD_DEFINITIONS( -Wall -Wundef -Wcast-align -Wconversion -Wchar-subscripts -Wpointer-arith 
		       -Wwrite-strings -Wpacked -Wformat-security -Wmissing-format-attribute 
		       -Wold-style-cast )
# to be added:       
#	-Woverloaded-virtual  -Wshadow 
#  -Wnon-virtual-dtor # Qt has too many of those already
   ENDIF (CMAKE_COMPILER_IS_GNUCXX)

   SET( CharmCore_SRCS 
		       Application.cpp
		       Charm.cpp
		       Controller.cpp
		       SqLiteStorage.cpp
		       TaskIdDialog.cpp
		       Configuration.cpp
		       Data.cpp
		       SqlStorage.cpp
		       modeltest.cpp
		       ConfigurationDialog.cpp 
		       CharmPreferences.cpp
		       Event.cpp
		       Task.cpp
		       State.cpp
		       View.cpp
		       StatusBarWidget.cpp
		       GUIState.cpp
		       EventEditor.cpp 
		       EventEditorDelegate.cpp 
		       SelectTaskDialog.cpp
		       Model.cpp
		       CharmDataModel.cpp
		       ViewFilter.cpp
		       TaskModelAdapter.cpp
		       EventModelAdapter.cpp
		       EventModelFilter.cpp
		       TaskTreeItem.cpp 
		       TimeSpans.cpp
		       ChattyItemDelegate.cpp 
		       CharmAboutDialog.cpp
		       Reports/ReportDialog.cpp
		       Reports/CharmReport.cpp
		       Reports/ActivityReport.cpp 
		       Reports/WeeklyTimeSheet.cpp
		       Reports/WeeklyTimeSheet-OldSchool.cpp
		       Reports/ReportPreviewWindow.cpp 
		       Reports/ParagraphFormatCollection.cpp

		       Commands/CharmCommand.cpp
		       Commands/CommandRelayCommand.cpp
		       Commands/CommandModifyEvent.cpp
		       Commands/CommandDeleteEvent.cpp
		       Commands/CommandAddTask.cpp
		       Commands/CommandModifyTask.cpp
		       Commands/CommandDeleteTask.cpp
		       Commands/CommandMakeEvent.cpp
		       Commands/CommandMakeAndActivateEvent.cpp )
    
    QT4_AUTOMOC ( ${CharmCore_SRCS} )

    QT4_WRAP_UI( UiGenerated_SRCS
    		       ConfigurationDialog.ui
		       CharmPreferences.ui
		       TaskIdDialog.ui
		       Reports/ReportDialog.ui
		       Reports/ActivityReportConfigurationPage.ui
		       Reports/WeeklyTimeSheetConfigurationPage.ui
		       Reports/ReportPreviewWindow.ui
		       View.ui
		       StatusBarWidget.ui
		       EventEditor.ui 
		       SelectTaskDialog.ui
		       CharmAboutDialog.ui )

    QT4_ADD_RESOURCES ( Resources_SRCS CharmResources.qrc )

    ADD_LIBRARY( CharmCore STATIC 
		       ${CharmCore_SRCS}
		       ${UiGenerated_SRCS} 
		       ${Resources_SRCS} )
ENDIF ( QT4_FOUND )

IF ( NOT QT4_FOUND )
   MESSAGE( "Fatal: Qt 4 not found." )
ENDIF ( NOT QT4_FOUND) 

