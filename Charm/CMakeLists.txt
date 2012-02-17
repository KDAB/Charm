INCLUDE_DIRECTORIES( ${Charm_SOURCE_DIR} ${Charm_BINARY_DIR} )

SET(
    CharmApplication_SRCS
    CharmWindow.cpp
    Application.cpp
    ApplicationFactory.cpp
    ViewHelpers.cpp
    TaskIdDialog.cpp
    Data.cpp
    ConfigurationDialog.cpp
    CharmPreferences.cpp
    TasksView.cpp
    TasksWindow.cpp
    TaskEditor.cpp
    TrayIcon.cpp
    GUIState.cpp
    DateEntrySyncer.cpp
    EnterVacationDialog.cpp
    EventView.cpp
    EventWindow.cpp
    EventEditor.cpp
    EventEditorDelegate.cpp
    SelectTaskDialog.cpp
    MessageBox.cpp
    ModelConnector.cpp
    ViewFilter.cpp
    TaskModelAdapter.cpp
    EventModelAdapter.cpp
    EventModelFilter.cpp
    CharmAboutDialog.cpp
    TimeTrackingView/TimeTrackingView.cpp
    TimeTrackingView/TimeTrackingWindow.cpp
    TimeTrackingView/WeeklySummary.cpp
    TimeTrackingView/TimeTrackingTaskSelector.cpp
    IdleWidget.cpp
    Reports/ReportConfigurationDialog.cpp
    Reports/CharmReport.cpp
    Reports/ActivityReport.cpp
    Reports/WeeklyTimesheet.cpp
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
    HttpClient/HttpJob.cpp
    HttpClient/GetProjectCodesJob.cpp
    HttpClient/UploadTimesheetJob.cpp
)

SET(IDLEDETECTION_LIBS)
IF( CHARM_IDLE_DETECTION )
    IF( APPLE )
        LIST( APPEND CharmApplication_SRCS Idle/MacIdleDetector.mm )
    ELSEIF( WIN32 )
        LIST( APPEND CharmApplication_SRCS Idle/WindowsIdleDetector.cpp )
    ELSEIF( UNIX )
        FIND_PACKAGE( X11 )
        IF( X11_FOUND AND X11_Xscreensaver_LIB )
            MESSAGE( "X11 idle detection enabled." )
            INCLUDE_DIRECTORIES( ${X11_INCLUDE_DIR} )
            LIST( APPEND CharmApplication_SRCS Idle/X11IdleDetector.cpp )
            SET( IDLEDETECTION_LIBS ${X11_X11_LIB} ${X11_Xscreensaver_LIB} )
            SET( CHARM_IDLE_DETECTION_AVAILABLE_X11 "1" CACHE INTERNAL "" )
        ELSE()
            MESSAGE( "Install X11/XScreenSaver headers and library for X11 idle detection." )
        ENDIF()
    ENDIF()
ENDIF()

LIST( APPEND CharmApplication_SRCS keychain.cpp )

IF (APPLE)
    LIST( APPEND CharmApplication_SRCS keychain_mac.cpp MacApplication.mm )
    SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework CoreFoundation -framework Security -framework AppKit" )
ELSEIF ( WIN32 )
    LIST( APPEND CharmApplication_SRCS keychain_win.cpp )
ELSEIF ( UNIX )
    LIST( APPEND CharmApplication_SRCS keychain_dbus.cpp )
ENDIF()

QT4_WRAP_CPP( MOC_QOCOA_SRCS Qocoa/qbutton.h Qocoa/qsearchfield.h )
LIST( APPEND CharmApplication_SRCS ${MOC_QOCOA_SRCS} )
IF (APPLE)
    LIST( APPEND CharmApplication_SRCS Qocoa/qbutton_mac.mm Qocoa/qsearchfield_mac.mm )
ELSE()
    LIST( APPEND CharmApplication_SRCS Qocoa/qbutton_nonmac.cpp Qocoa/qsearchfield_nonmac.cpp )
ENDIF()

QT4_AUTOMOC( ${CharmApplication_SRCS} )

QT4_WRAP_UI(
    UiGenerated_SRCS
    ConfigurationDialog.ui
    CharmPreferences.ui
    TaskIdDialog.ui
    Reports/ActivityReportConfigurationDialog.ui
    Reports/WeeklyTimesheetConfigurationDialog.ui
    Reports/ReportPreviewWindow.ui
    TaskEditor.ui
    EnterVacationDialog.ui
    EventEditor.ui
    SelectTaskDialog.ui
    CharmAboutDialog.ui
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
    SET( MACOSX_BUNDLE_COPYRIGHT "2006-2011 KDAB" )
    SET( MACOSX_BUNDLE_ICON_FILE "Charm.icns" )
    SET( MACOSX_BUNDLE_GUI_IDENTIFIER "com.kdab" )
    SET( MACOSX_BUNDLE_BUNDLE_NAME "Charm" )

    SET( RESOURCES "${CMAKE_CURRENT_BINARY_DIR}/Charm.app/Contents/Resources"	)
    SET( ICON "${ICONS_DIR}/${MACOSX_BUNDLE_ICON_FILE}" )
    FILE( MAKE_DIRECTORY ${RESOURCES} )
    FILE( COPY ${ICON} DESTINATION ${RESOURCES} )
ENDIF()

IF( MSVC )
    SET( Resources_SRCS ${Resources_SRCS} Charm.rc )
ENDIF()

ADD_EXECUTABLE( ${Charm_EXECUTABLE} WIN32 MACOSX_BUNDLE ${Charm_SRCS} ${Resources_SRCS} )
TARGET_LINK_LIBRARIES( ${Charm_EXECUTABLE} CharmApplication CharmCore ${QT_LIBRARIES} )
IF( WIN32 )
    TARGET_LINK_LIBRARIES( ${Charm_EXECUTABLE} Crypt32 )
ENDIF()
MESSAGE( STATUS "Charm will be installed to ${CMAKE_INSTALL_PREFIX}" )

IF( UNIX AND NOT APPLE )
    SET( XDG_APPS_INSTALL_DIR share/applications )
    INSTALL( FILES Charm.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
    INSTALL( FILES Icons/Charm-128x128.png DESTINATION share/icons/hicolor/128x128/apps )
    INSTALL( FILES Icons/tray/charmtray16.png DESTINATION share/icons/hicolor/16x16/apps )
    INSTALL( FILES Icons/tray/charmtray24.png DESTINATION share/icons/hicolor/24x24/apps )
    INSTALL( FILES Icons/tray/01.png Icons/tray/02.png Icons/tray/03.png
        Icons/tray/04.png Icons/tray/05.png Icons/tray/06.png
        Icons/tray/07.png Icons/tray/08.png Icons/tray/09.png
        Icons/tray/10.png Icons/tray/11.png Icons/tray/12.png
    DESTINATION share/icons/hicolor/24x24/apps )
ENDIF()

INSTALL( TARGETS ${Charm_EXECUTABLE} DESTINATION ${BIN_INSTALL_DIR} )
IF( APPLE )
    SET( EXECUTABLE ${Charm_EXECUTABLE}.app )
ELSE()
    SET( EXECUTABLE ${Charm_EXECUTABLE}${CMAKE_EXECUTABLE_SUFFIX} )
ENDIF()

EXECUTE_PROCESS(
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE} ${Charm_BINARY_DIR}/${EXECUTABLE}
)

# Only support CPack packaging on newer versions of CMake.
IF( NOT "${CMAKE_VERSION}" VERSION_LESS "2.8.4" )
    IF( CMAKE_BUILD_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])" )
        SET( CMAKE_INSTALL_DEBUG_LIBRARIES_ONLY TRUE )
    ENDIF()

    SET( CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION "${BIN_INSTALL_DIR}" )
    INCLUDE( InstallRequiredSystemLibraries )

    IF( NOT BIN_INSTALL_DIR STREQUAL "." )
        SET( EXECUTABLE ${BIN_INSTALL_DIR}/${EXECUTABLE} )
    ENDIF()

    IF( APPLE OR WIN32 )
        INCLUDE( DeployQt4 )
        INSTALL_QT4_EXECUTABLE( "${EXECUTABLE}" "qsqlite" )
    ENDIF()
ENDIF()
