INCLUDE_DIRECTORIES( ${Charm_SOURCE_DIR} ${Charm_BINARY_DIR} )

SET(
    CharmApplication_SRCS
    ApplicationCore.cpp
    Data.cpp
    EventModelAdapter.cpp
    EventModelFilter.cpp
    GUIState.cpp
    ModelConnector.cpp
    ViewFilter.cpp
    TaskModelAdapter.cpp
    ViewHelpers.cpp
    WeeklySummary.cpp
    UndoCharmCommandWrapper.cpp
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
    HttpClient/HttpJob.cpp
    HttpClient/GetProjectCodesJob.cpp
    HttpClient/UploadTimesheetJob.cpp
    HttpClient/GetUserInfoJob.cpp
    HttpClient/CheckForUpdatesJob.cpp
    Idle/IdleDetector.cpp
    Reports/TimesheetInfo.cpp
    Reports/MonthlyTimesheetXmlWriter.cpp
    Reports/WeeklyTimesheetXmlWriter.cpp
    Widgets/ActivityReport.cpp
    Widgets/BillDialog.cpp
    Widgets/CharmPreferences.cpp
    Widgets/CharmWindow.cpp
    Widgets/CharmAboutDialog.cpp
    Widgets/CharmNewReleaseDialog.cpp
    Widgets/CommentEditorPopup.cpp
    Widgets/ConfigurationDialog.cpp
    Widgets/DateEntrySyncer.cpp
    Widgets/EnterVacationDialog.cpp
    Widgets/EventEditor.cpp
    Widgets/EventEditorDelegate.cpp
    Widgets/EventView.cpp
    Widgets/ExpandStatesHelper.cpp
    Widgets/HttpJobProgressDialog.cpp
    Widgets/IdleCorrectionDialog.cpp
    Widgets/MessageBox.cpp
    Widgets/MonthlyTimesheet.cpp
    Widgets/MonthlyTimesheetConfigurationDialog.cpp
    Widgets/ReportConfigurationDialog.cpp
    Widgets/ReportPreviewWindow.cpp
    Widgets/SelectTaskDialog.cpp
    Widgets/TaskIdDialog.cpp
    Widgets/TaskEditor.cpp
    Widgets/TasksView.cpp
    Widgets/TasksViewDelegate.cpp
    Widgets/TimeTrackingView.cpp
    Widgets/TimeTrackingWindow.cpp
    Widgets/TimeTrackingTaskSelector.cpp
    Widgets/TrayIcon.cpp
    Widgets/Timesheet.cpp
    Widgets/WeeklyTimesheet.cpp
    Widgets/NotificationPopup.cpp
    Widgets/FindAndReplaceEventsDialog.cpp
    Widgets/WidgetUtils.cpp
)

SET(CharmApplication_LIBS)

IF( CHARM_CI_SUPPORT )
    LIST( APPEND CharmApplication_SRCS
        CI/CharmCommandInterface.cpp
        CI/CharmCommandServer.cpp
        CI/CharmCommandSession.cpp
        )
    IF( CHARM_CI_LOCALSERVER )
        LIST( APPEND CharmApplication_SRCS CI/CharmLocalCommandServer.cpp )
    ENDIF()
    IF( CHARM_CI_TCPSERVER )
        LIST( APPEND CharmApplication_SRCS CI/CharmTCPCommandServer.cpp )
    ENDIF()
ENDIF()

IF( CHARM_IDLE_DETECTION )
    IF( APPLE )
        LIST( APPEND CharmApplication_SRCS Idle/MacIdleDetector.mm )
    ELSEIF( WIN32 )
        LIST( APPEND CharmApplication_SRCS Idle/WindowsIdleDetector.cpp )
    ELSEIF( UNIX )
        SET( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/CMake/Modules/ )
        IF( Qt5Core_FOUND )
            FIND_PACKAGE( XCB )
            IF( XCB_INCLUDE_PATH_FOUND AND XCB_LIBRARIES_FOUND AND XCB_SCREENSAVER_LIBRARIES_FOUND  )
                INCLUDE_DIRECTORIES( ${XCB_INCLUDE_DIRS} )
                LIST( APPEND CharmApplication_LIBS ${XCB_LIBRARIES} ${XCB_SCREENSAVER_LIBRARIES} )
                LIST( APPEND CharmApplication_SRCS Idle/X11IdleDetector.cpp )
                SET( CHARM_IDLE_DETECTION_AVAILABLE "1" CACHE INTERNAL "" )
            ELSE()
                SET( CHARM_IDLE_DETECTION_AVAILABLE "0" CACHE INTERNAL "" )
                MESSAGE( "QT5: Install Xcb headers and library for Xcb idle detection." )
            ENDIF()
        ELSE()
            FIND_PACKAGE( X11 )
            IF(  X11_FOUND AND X11_Xscreensaver_LIB)
                INCLUDE_DIRECTORIES( ${X11_INCLUDE_DIR} )
                LIST( APPEND CharmApplication_LIBS ${X11_X11_LIB} ${X11_Xscreensaver_LIB} )
                LIST( APPEND CharmApplication_SRCS Idle/X11IdleDetector.cpp )
                SET( CHARM_IDLE_DETECTION_AVAILABLE "1" CACHE INTERNAL "" )
            ELSE()
                SET( CHARM_IDLE_DETECTION_AVAILABLE "0" CACHE INTERNAL "" )
                MESSAGE( "QT4: Install X11/XScreenSaver headers and library for X11 idle detection." )
            ENDIF()
        ENDIF()
    ENDIF()
ENDIF()

IF (NOT QTKEYCHAIN_SYSTEM)
	LIST( APPEND CharmApplication_SRCS Keychain/keychain.cpp )
	IF (APPLE)
	    LIST( APPEND CharmApplication_SRCS Keychain/keychain_mac.cpp MacApplicationCore.mm )

	    FIND_LIBRARY( COREFOUNDATION_LIBRARY CoreFoundation )
	    LIST( APPEND CharmApplication_LIBS ${COREFOUNDATION_LIBRARY} )

	    FIND_LIBRARY( SECURITY_LIBRARY Security )
	    LIST( APPEND CharmApplication_LIBS ${SECURITY_LIBRARY} )

	    FIND_LIBRARY( APPKIT_LIBRARY AppKit )
	    LIST( APPEND CharmApplication_LIBS ${APPKIT_LIBRARY} )
	ELSEIF ( WIN32 )
	    LIST( APPEND CharmApplication_SRCS Keychain/keychain_win.cpp )
	ELSEIF ( UNIX )
	    IF (HAVE_DBUS)
		LIST( APPEND CharmApplication_SRCS Keychain/gnomekeyring.cpp Keychain/keychain_unix.cpp )
                QT5_ADD_DBUS_INTERFACE( CharmApplication_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/Keychain/org.kde.KWallet.xml kwallet_interface KWalletInterface )
                LIST( APPEND CharmApplication_LIBS Qt5::DBus )
	    ELSE()
		LIST( APPEND CharmApplication_SRCS Keychain/keychain_unsecure.cpp )
	    ENDIF()
	ENDIF()
ENDIF()

QT5_WRAP_UI(
    UiGenerated_SRCS
    Widgets/CommentEditorPopup.ui
    Widgets/ConfigurationDialog.ui
    Widgets/CharmPreferences.ui
    Widgets/TaskIdDialog.ui
    Widgets/TaskEditor.ui
    Widgets/EnterVacationDialog.ui
    Widgets/EventEditor.ui
    Widgets/SelectTaskDialog.ui
    Widgets/CharmAboutDialog.ui
    Widgets/CharmNewReleaseDialog.ui
    Widgets/IdleCorrectionDialog.ui
    Widgets/ActivityReportConfigurationDialog.ui
    Widgets/WeeklyTimesheetConfigurationDialog.ui
    Widgets/MonthlyTimesheetConfigurationDialog.ui
    Widgets/ReportPreviewWindow.ui
    Widgets/NotificationPopup.ui
    Widgets/FindAndReplaceEventsDialog.ui
)

QT5_ADD_RESOURCES( Resources_SRCS CharmResources.qrc )

ADD_LIBRARY(
    CharmApplication STATIC
    ${CharmApplication_SRCS} ${UiGenerated_SRCS}
)

kde_target_enable_exceptions( CharmApplication PUBLIC )
TARGET_LINK_LIBRARIES(CharmApplication ${CharmApplication_LIBS}
    Qt5::Core
    Qt5::Widgets
    Qt5::Xml
    Qt5::Network
    Qt5::Script
    Qt5::Sql
    Qt5::PrintSupport)

IF (WIN32)
    TARGET_LINK_LIBRARIES(CharmApplication
        Qt5::WinExtras Crypt32)
ENDIF()

IF (QTKEYCHAIN_SYSTEM)
    TARGET_LINK_LIBRARIES(CharmApplication qt5keychain)
ENDIF()

SET( Charm_SRCS Charm.cpp )

ecm_add_app_icon(Charm_SRCS ICONS Icons/128-apps-Charm.png)


IF( APPLE )
    IF( CHARM_MAC_HIGHRES_SUPPORT_ENABLED )
        SET( MACOSX_BUNDLE_HIGHRESOLUTION_CAPABLE "true")
    ELSE()
        SET( MACOSX_BUNDLE_HIGHRESOLUTION_CAPABLE "false")
    ENDIF()
    SET( MACOSX_BUNDLE_INFO_STRING "Charm ${Charm_VERSION}" )
    SET( MACOSX_BUNDLE_BUNDLE_VERSION "Charm ${Charm_VERSION}" )
    SET( MACOSX_BUNDLE_LONG_VERSION_STRING "Charm ${Charm_VERSION}" )
    SET( MACOSX_BUNDLE_SHORT_VERSION_STRING "${Charm_VERSION}" )
    SET( MACOSX_BUNDLE_COPYRIGHT "2006-2014 KDAB" )
    SET( MACOSX_BUNDLE_ICON_FILE "Charm.icns" )
    SET( MACOSX_BUNDLE_GUI_IDENTIFIER "com.kdab" )
    SET( MACOSX_BUNDLE_BUNDLE_NAME "Charm" )

    SET( RESOURCES "${CMAKE_CURRENT_BINARY_DIR}/Charm.app/Contents/Resources"	)
    SET( ICON "${ICONS_DIR}/${MACOSX_BUNDLE_ICON_FILE}" )
    FILE( MAKE_DIRECTORY ${RESOURCES} )
    FILE( COPY ${ICON} DESTINATION ${RESOURCES} )
ENDIF()

ADD_EXECUTABLE( ${Charm_EXECUTABLE} ${Charm_SRCS} ${Resources_SRCS} )
kde_target_enable_exceptions( ${Charm_EXECUTABLE} PUBLIC )
TARGET_LINK_LIBRARIES( ${Charm_EXECUTABLE} CharmApplication CharmCore)

MESSAGE( STATUS "Charm will be installed to ${CMAKE_INSTALL_PREFIX}" )

IF( UNIX AND NOT APPLE )
    INSTALL( FILES charmtimetracker.desktop DESTINATION ${CMAKE_INSTALL_APPDIR})

    ecm_install_icons( ICONS Icons/16-apps-Charm.png
                             Icons/32-apps-Charm.png
                             Icons/48-apps-Charm.png
                             Icons/64-apps-Charm.png
                             Icons/128-apps-Charm.png
                             Icons/256-apps-Charm.png
                       DESTINATION
                            ${CMAKE_INSTALL_ICONDIR})
ENDIF()

INSTALL( TARGETS ${Charm_EXECUTABLE} ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )
IF( APPLE )
    SET( EXECUTABLE ${Charm_EXECUTABLE}.app )
    set_target_properties( ${Charm_EXECUTABLE} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/MacOSXBundleInfo.plist.in )
ELSE()
    SET( EXECUTABLE ${Charm_EXECUTABLE}${CMAKE_EXECUTABLE_SUFFIX} )
ENDIF()

IF(NOT WIN32)
    EXECUTE_PROCESS(
        COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE} ${Charm_BINARY_DIR}/${EXECUTABLE}
    )
ENDIF()

# Only support CPack packaging on newer versions of CMake.
IF( CHARM_PREPARE_DEPLOY )
    IF( CMAKE_BUILD_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])" )
        SET( CMAKE_INSTALL_DEBUG_LIBRARIES_ONLY TRUE )
    ENDIF()

    SET( CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION "${BIN_INSTALL_DIR}" )
    INCLUDE( InstallRequiredSystemLibraries )

    IF( NOT BIN_INSTALL_DIR STREQUAL "." )
        SET( EXECUTABLE ${BIN_INSTALL_DIR}/${EXECUTABLE} )
    ENDIF()

    IF( WIN32 )
        FIND_PACKAGE( OpenSSL REQUIRED )
        INSTALL( FILES ${OPENSSL_INCLUDE_DIR}/../libeay32.dll ${OPENSSL_INCLUDE_DIR}/../ssleay32.dll
                 DESTINATION ${BIN_INSTALL_DIR} )
    ENDIF()

    # Plugin deployment for Windows
    IF( WIN32)
        # Deploy platform plugin to platforms/ (plugins/platforms isn't found, for whatever reason)
        GET_TARGET_PROPERTY(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
        EXEC_PROGRAM( ${_qmake_executable} ARGS -query QT_INSTALL_PLUGINS OUTPUT_VARIABLE _loc )
        INSTALL( FILES ${_loc}/platforms/qwindows.dll DESTINATION platforms )
        # Deploy qsqlite plugin
        GET_TARGET_PROPERTY( _loc Qt5::QSQLiteDriverPlugin LOCATION )
        INSTALL( FILES ${_loc} DESTINATION plugins/sqldrivers )
        # Deploy windowsprintersupport plugin
        GET_TARGET_PROPERTY( _loc Qt5::QWindowsPrinterSupportPlugin LOCATION )
        INSTALL( FILES ${_loc} DESTINATION plugins/printsupport )
        # Install qt.conf (prevents plugins from the built-in Qt path to be used)
        INSTALL(FILES qt.conf.in.windows RENAME qt.conf DESTINATION .)
    ENDIF()
ENDIF()
