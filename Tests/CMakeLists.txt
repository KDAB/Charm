INCLUDE_DIRECTORIES( ${Charm_SOURCE_DIR} )

if(POLICY CMP0020)
    CMAKE_POLICY(SET CMP0020 NEW)
endif()

SET( TestApplication_SRCS TestApplication.cpp )

SET( TEST_LIBRARIES CharmCore ${QT_QTTEST_LIBRARY} ${QT_LIBRARIES} )

QT_ADD_RESOURCES( TestDataResources_SRCS TestData.qrc )

IF( APPLE )
    FIND_LIBRARY( SECURITY_FRAMEWORK FRAMEWORK required NAMES Security )
    LIST( APPEND TEST_LIBRARIES ${SECURITY_FRAMEWORK} )
ENDIF()

SET( SqLiteStorageTests_SRCS SqLiteStorageTests.cpp )
ADD_EXECUTABLE( SqLiteStorageTests ${SqLiteStorageTests_SRCS} )
TARGET_LINK_LIBRARIES( SqLiteStorageTests ${TEST_LIBRARIES} )
ADD_TEST( NAME SqLiteStorageTests COMMAND SqLiteStorageTests )

SET( ControllerTests_SRCS ControllerTests.cpp )
ADD_EXECUTABLE( ControllerTests ${ControllerTests_SRCS} )
TARGET_LINK_LIBRARIES( ControllerTests ${TEST_LIBRARIES} )
ADD_TEST( NAME ControllerTests COMMAND ControllerTests )

SET( DatesTests_SRCS DatesTests.cpp )
ADD_EXECUTABLE( DatesTests ${DatesTests_SRCS} )
TARGET_LINK_LIBRARIES( DatesTests ${TEST_LIBRARIES} )

SET( SmartNameCacheTests_SRCS SmartNameCacheTests.cpp )
ADD_EXECUTABLE( SmartNameCacheTests ${SmartNameCacheTests_SRCS} )
TARGET_LINK_LIBRARIES( SmartNameCacheTests ${TEST_LIBRARIES} )

SET( CharmDataModelTests_SRCS CharmDataModelTests.cpp )
ADD_EXECUTABLE( CharmDataModelTests ${CharmDataModelTests_SRCS} )
TARGET_LINK_LIBRARIES( CharmDataModelTests ${TEST_LIBRARIES} )
ADD_TEST( NAME CharmDataModelTests COMMAND CharmDataModelTests )

SET(
    BackendIntegrationTests_SRCS
    BackendIntegrationTests.cpp
    ${TestApplication_SRCS}
)
ADD_EXECUTABLE( BackendIntegrationTests ${BackendIntegrationTests_SRCS} )
TARGET_LINK_LIBRARIES( BackendIntegrationTests ${TEST_LIBRARIES} )
ADD_TEST( NAME BackendIntegrationTests COMMAND BackendIntegrationTests )

SET( TaskStructureTests_SRCS TaskStructureTests.cpp )
ADD_EXECUTABLE(
    TaskStructureTests
    ${TaskStructureTests_SRCS}
    ${TestDataResources_SRCS}
)
TARGET_LINK_LIBRARIES( TaskStructureTests ${TEST_LIBRARIES} )
ADD_TEST( NAME TaskStructureTests COMMAND TaskStructureTests )

SET( TimeSpanTests_SRCS TimeSpanTests.cpp )
ADD_EXECUTABLE( TimeSpanTests ${TimeSpanTests_SRCS} )
TARGET_LINK_LIBRARIES( TimeSpanTests ${TEST_LIBRARIES} )

SET( XmlSerializationTests_SRCS XmlSerializationTests.cpp )
ADD_EXECUTABLE(
    XmlSerializationTests
    ${XmlSerializationTests_SRCS}
    ${TestDataResources_SRCS}
)
TARGET_LINK_LIBRARIES( XmlSerializationTests ${TEST_LIBRARIES} )
ADD_TEST( NAME XmlSerializationTests COMMAND XmlSerializationTests )

SET( ImportExportTests_SRCS ImportExportTests.cpp ${TestApplication_SRCS} )
ADD_EXECUTABLE(
    ImportExportTests
    ${ImportExportTests_SRCS}
    ${TestDataResources_SRCS}
)
TARGET_LINK_LIBRARIES( ImportExportTests ${TEST_LIBRARIES} )
ADD_TEST( NAME ImportExportTests COMMAND ImportExportTests )

SET( SqlTransactionTests_SRCS SqlTransactionTests.cpp )
ADD_EXECUTABLE( SqlTransactionTests ${SqlTransactionTests_SRCS} )
TARGET_LINK_LIBRARIES( SqlTransactionTests ${TEST_LIBRARIES} )

IF( CHARM_DATABASE_CONFIGURATION )
    ADD_TEST( NAME SqlTransactionTests COMMAND SqlTransactionTests )
    SET_PROPERTY(
        TEST SqlTransactionTests
        PROPERTY ENVIRONMENT
        "CHARM_DATABASE_CONFIGURATION=${CHARM_DATABASE_CONFIGURATION}"
    )

    SET(TimeSheetProcessorTests_SRCS TimeSheetProcessorTests.cpp
                                     ${Charm_SOURCE_DIR}/Tools/TimesheetProcessor/Operations.cpp
                                     ${Charm_SOURCE_DIR}/Tools/TimesheetProcessor/CommandLine.cpp
                                     ${Charm_SOURCE_DIR}/Tools/TimesheetProcessor/Database.cpp
    )
    ADD_EXECUTABLE(TimeSheetProcessorTests
                   ${TimeSheetProcessorTests_SRCS}
                   ${TestDataResources_SRCS}
    )
    TARGET_LINK_LIBRARIES(TimeSheetProcessorTests ${TEST_LIBRARIES} )
    ADD_TEST(NAME TimeSheetProcessorTests COMMAND TimeSheetProcessorTests )

ENDIF()

SET( UpdateCheckerTests_SRCS ${Charm_SOURCE_DIR}/Charm/HttpClient/CheckForUpdatesJob.cpp UpdateCheckerTests.cpp )
ADD_EXECUTABLE( UpdateCheckerTests ${UpdateCheckerTests_SRCS} )
TARGET_LINK_LIBRARIES( UpdateCheckerTests ${TEST_LIBRARIES} )
