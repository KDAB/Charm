SET( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

IF ( QT4_FOUND )
   INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} )
   INCLUDE_DIRECTORIES( ${QT_QTCORE_INCLUDE_DIR} )
   INCLUDE_DIRECTORIES( ${QT_QTGUI_INCLUDE_DIR} )
   INCLUDE_DIRECTORIES( ${QT_QTSQL_INCLUDE_DIR} )
   INCLUDE_DIRECTORIES( ${QT_QTXML_INCLUDE_DIR} )
   INCLUDE_DIRECTORIES( ../ )
   include (${QT_USE_FILE})

   LINK_DIRECTORIES( ${Charm_SOURCE_DIR}/Core )
   LINK_LIBRARIES( CharmCore )
   LINK_LIBRARIES( ${QT_QTCORE_LIBRARY} )
   IF( APPLE )
       FIND_LIBRARY( SECURITY FRAMEWORK required NAMES Security )
       LINK_LIBRARIES( ${SECURITY} )
   ENDIF( APPLE )

   # SqLiteStorageTests

   SET( SqLiteStorageTests_SRCS SqLiteStorageTests.cpp )

   QT4_AUTOMOC( ${SqLiteStorageTests_SRCS} )

   ADD_EXECUTABLE( SqLiteStorageTests ${SqLiteStorageTests_SRCS} )

   TARGET_LINK_LIBRARIES( SqLiteStorageTests ${QT_QTTEST_LIBRARY} )
   TARGET_LINK_LIBRARIES( SqLiteStorageTests ${QT_LIBRARIES} )

   # ControllerTests

   SET( ControllerTests_SRCS ControllerTests.cpp )

   QT4_AUTOMOC( ${ControllerTests_SRCS} )

   ADD_EXECUTABLE( ControllerTests ${ControllerTests_SRCS} )

   TARGET_LINK_LIBRARIES( ControllerTests ${QT_QTTEST_LIBRARY} )
   TARGET_LINK_LIBRARIES( ControllerTests ${QT_LIBRARIES} )

   # CharmDataModelTests

   SET( CharmDataModelTests_SRCS CharmDataModelTests.cpp )

   QT4_AUTOMOC( ${CharmDataModelTests_SRCS} )

   ADD_EXECUTABLE( CharmDataModelTests ${CharmDataModelTests_SRCS} )

   TARGET_LINK_LIBRARIES( CharmDataModelTests ${QT_QTTEST_LIBRARY} )
   TARGET_LINK_LIBRARIES( CharmDataModelTests ${QT_LIBRARIES} )

   # BackendIntegrationTests
   SET( BackendIntegrationTests_SRCS BackendIntegrationTests.cpp )

   QT4_AUTOMOC( ${BackendIntegrationTests_SRCS} )

   ADD_EXECUTABLE( BackendIntegrationTests ${BackendIntegrationTests_SRCS} )

   TARGET_LINK_LIBRARIES( BackendIntegrationTests ${QT_QTTEST_LIBRARY} )
   TARGET_LINK_LIBRARIES( BackendIntegrationTests ${QT_LIBRARIES} )

   # Task Structure Tests:
   SET( TaskStructureTests_SRCS TaskStructureTests.cpp )

   QT4_AUTOMOC( ${TaskStructureTests_SRCS} )
   QT4_ADD_RESOURCES ( TaskStructureTestsResources_SRCS TestData.qrc )

   ADD_EXECUTABLE( TaskStructureTests ${TaskStructureTests_SRCS} ${TaskStructureTestsResources_SRCS} )

   TARGET_LINK_LIBRARIES( TaskStructureTests ${QT_QTTEST_LIBRARY} )
   TARGET_LINK_LIBRARIES( TaskStructureTests ${QT_LIBRARIES} )

   # XML Serialization Tests:
   SET( XmlSerializationTests_SRCS XmlSerializationTests.cpp )

   QT4_AUTOMOC( ${XmlSerializationTests_SRCS} )

   ADD_EXECUTABLE( XmlSerializationTests ${XmlSerializationTests_SRCS} ${TaskStructureTestsResources_SRCS} )

   TARGET_LINK_LIBRARIES( XmlSerializationTests ${QT_QTTEST_LIBRARY} )
   TARGET_LINK_LIBRARIES( XmlSerializationTests ${QT_QTXML_LIBRARY} )
   TARGET_LINK_LIBRARIES( XmlSerializationTests ${QT_LIBRARIES} )

   # Database Import/Export Tests:
   SET( ImportExportTests_SRCS ImportExportTests.cpp )

   QT4_AUTOMOC( ${ImportExportTests_SRCS} )

   ADD_EXECUTABLE( ImportExportTests ${ImportExportTests_SRCS} ${TaskStructureTestsResources_SRCS} )

   TARGET_LINK_LIBRARIES( ImportExportTests ${QT_QTTEST_LIBRARY} )
   TARGET_LINK_LIBRARIES( ImportExportTests ${QT_QTXML_LIBRARY} )
   TARGET_LINK_LIBRARIES( ImportExportTests ${QT_LIBRARIES} )

   # Sql Transaction Tests:
   SET( SqlTransactionTests_SRCS SqlTransactionTests.cpp )

   QT4_AUTOMOC( ${SqlTransactionTests_SRCS} )

   ADD_EXECUTABLE( SqlTransactionTests ${SqlTransactionTests_SRCS} )

   TARGET_LINK_LIBRARIES( SqlTransactionTests ${QT_QTTEST_LIBRARY} )
   TARGET_LINK_LIBRARIES( SqlTransactionTests ${QT_QTXML_LIBRARY} )
   TARGET_LINK_LIBRARIES( SqlTransactionTests ${QT_LIBRARIES} )

   # now add the tests:
   ADD_TEST( SqLiteStorageTests ${EXECUTABLE_OUTPUT_PATH}/SqLiteStorageTests )
   ADD_TEST( ControllerTests ${EXECUTABLE_OUTPUT_PATH}/ControllerTests )
   ADD_TEST( CharmDataModelTests ${EXECUTABLE_OUTPUT_PATH}/CharmDataModelTests )
   ADD_TEST( BackendIntegrationTests ${EXECUTABLE_OUTPUT_PATH}/BackendIntegrationTests )
   ADD_TEST( XmlSerializationTests ${EXECUTABLE_OUTPUT_PATH}/XmlSerializationTests )
   ADD_TEST( TaskStructureTests ${EXECUTABLE_OUTPUT_PATH}/TaskStructureTests )
   ADD_TEST( ImportExportTests ${EXECUTABLE_OUTPUT_PATH}/ImportExportTests )
   IF ( CHARM_DATABASE_CONFIGURATION )
       ADD_TEST( SqlTransactionTests ${EXECUTABLE_OUTPUT_PATH}/SqlTransactionTests )
       SET_PROPERTY( TEST SqlTransactionTests PROPERTY ENVIRONMENT "CHARM_DATABASE_CONFIGURATION=${CHARM_DATABASE_CONFIGURATION}" )
   ENDIF ( CHARM_DATABASE_CONFIGURATION )

ENDIF ( QT4_FOUND )

IF ( NOT QT4_FOUND )
   MESSAGE( "Fatal: Qt 4 not found." )
ENDIF ( NOT QT4_FOUND) 

