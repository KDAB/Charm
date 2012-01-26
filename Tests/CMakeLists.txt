INCLUDE_DIRECTORIES( ${Charm_SOURCE_DIR} )

SET( TestApplication_SRCS TestApplication.cpp )
SET( TEST_LIBRARIES CharmCore ${QT_QTTEST_LIBRARY} ${QT_LIBRARIES} )
QT4_ADD_RESOURCES( TestDataResources_SRCS TestData.qrc )

IF( APPLE )
    FIND_LIBRARY( SECURITY_FRAMEWORK FRAMEWORK required NAMES Security )
    LIST( APPEND TEST_LIBRARIES ${SECURITY_FRAMEWORK} )
ENDIF()

SET( SqLiteStorageTests_SRCS SqLiteStorageTests.cpp )
QT4_AUTOMOC( ${SqLiteStorageTests_SRCS} )
ADD_EXECUTABLE( SqLiteStorageTests ${SqLiteStorageTests_SRCS} )
TARGET_LINK_LIBRARIES( SqLiteStorageTests ${TEST_LIBRARIES} )
ADD_TEST( NAME SqLiteStorageTests COMMAND SqLiteStorageTests )

SET( ControllerTests_SRCS ControllerTests.cpp )
QT4_AUTOMOC( ${ControllerTests_SRCS} )
ADD_EXECUTABLE( ControllerTests ${ControllerTests_SRCS} )
TARGET_LINK_LIBRARIES( ControllerTests ${TEST_LIBRARIES} )
ADD_TEST( NAME ControllerTests COMMAND ControllerTests )

SET( DatesTests_SRCS DatesTests.cpp )
QT4_AUTOMOC( ${DatesTests_SRCS} )
ADD_EXECUTABLE( DatesTests ${DatesTests_SRCS} )
TARGET_LINK_LIBRARIES( DatesTests ${TEST_LIBRARIES} )

SET( CharmDataModelTests_SRCS CharmDataModelTests.cpp )
QT4_AUTOMOC( ${CharmDataModelTests_SRCS} )
ADD_EXECUTABLE( CharmDataModelTests ${CharmDataModelTests_SRCS} )
TARGET_LINK_LIBRARIES( CharmDataModelTests ${TEST_LIBRARIES} )
ADD_TEST( NAME CharmDataModelTests COMMAND CharmDataModelTests )

SET(
    BackendIntegrationTests_SRCS
    BackendIntegrationTests.cpp
    ${TestApplication_SRCS}
)
QT4_AUTOMOC( ${BackendIntegrationTests_SRCS} )
ADD_EXECUTABLE( BackendIntegrationTests ${BackendIntegrationTests_SRCS} )
TARGET_LINK_LIBRARIES( BackendIntegrationTests ${TEST_LIBRARIES} )
ADD_TEST( NAME BackendIntegrationTests COMMAND BackendIntegrationTests )

SET( TaskStructureTests_SRCS TaskStructureTests.cpp )
QT4_AUTOMOC( ${TaskStructureTests_SRCS} )
ADD_EXECUTABLE(
    TaskStructureTests
    ${TaskStructureTests_SRCS}
    ${TestDataResources_SRCS}
)
TARGET_LINK_LIBRARIES( TaskStructureTests ${TEST_LIBRARIES} )
ADD_TEST( NAME TaskStructureTests COMMAND TaskStructureTests )

SET( TimeSpanTests_SRCS TimeSpanTests.cpp )
QT4_AUTOMOC( ${TimeSpanTests_SRCS} )
ADD_EXECUTABLE( TimeSpanTests ${TimeSpanTests_SRCS} )
TARGET_LINK_LIBRARIES( TimeSpanTests ${TEST_LIBRARIES} )

SET( XmlSerializationTests_SRCS XmlSerializationTests.cpp )
QT4_AUTOMOC( ${XmlSerializationTests_SRCS} )
ADD_EXECUTABLE(
    XmlSerializationTests
    ${XmlSerializationTests_SRCS}
    ${TestDataResources_SRCS}
)
TARGET_LINK_LIBRARIES( XmlSerializationTests ${TEST_LIBRARIES} )
ADD_TEST( NAME XmlSerializationTests COMMAND XmlSerializationTests )

SET( ImportExportTests_SRCS ImportExportTests.cpp ${TestApplication_SRCS} )
QT4_AUTOMOC( ${ImportExportTests_SRCS} )
ADD_EXECUTABLE(
    ImportExportTests
    ${ImportExportTests_SRCS}
    ${TestDataResources_SRCS}
)
TARGET_LINK_LIBRARIES( ImportExportTests ${TEST_LIBRARIES} )
ADD_TEST( NAME ImportExportTests COMMAND ImportExportTests )

SET( SqlTransactionTests_SRCS SqlTransactionTests.cpp )
QT4_AUTOMOC( ${SqlTransactionTests_SRCS} )
ADD_EXECUTABLE( SqlTransactionTests ${SqlTransactionTests_SRCS} )
TARGET_LINK_LIBRARIES( SqlTransactionTests ${TEST_LIBRARIES} )
IF( CHARM_DATABASE_CONFIGURATION )
    ADD_TEST( NAME SqlTransactionTests COMMAND SqlTransactionTests )
    SET_PROPERTY(
        TEST SqlTransactionTests
        PROPERTY ENVIRONMENT
        "CHARM_DATABASE_CONFIGURATION=${CHARM_DATABASE_CONFIGURATION}"
    )
ENDIF()
