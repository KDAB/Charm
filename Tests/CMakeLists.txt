SET( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

IF ( QT4_FOUND )
   INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} )
   INCLUDE_DIRECTORIES( ${QT_QTCORE_INCLUDE_DIR} )
   INCLUDE_DIRECTORIES( ${QT_QTGUI_INCLUDE_DIR} )
   INCLUDE_DIRECTORIES( ${QT_QTSQL_INCLUDE_DIR} )
   INCLUDE_DIRECTORIES( ${QT_QTXML_INCLUDE_DIR} )
   INCLUDE_DIRECTORIES( ../Core )
   include (${QT_USE_FILE})

   IF (CMAKE_COMPILER_IS_GNUCXX)
      ADD_DEFINITIONS( -Wall -Wundef -Wcast-align -Wconversion -Wchar-subscripts -Wpointer-arith 
		       -Wwrite-strings -Wpacked -Wformat-security -Wmissing-format-attribute 
		       -Wold-style-cast )
# to be added:       
#	-Woverloaded-virtual  -Wshadow 
#  -Wnon-virtual-dtor # Qt has too many of those already
   ENDIF (CMAKE_COMPILER_IS_GNUCXX)

   LINK_DIRECTORIES( ../Core )
   LINK_LIBRARIES( CharmCore )
   LINK_LIBRARIES( ${QT_QTCORE_LIBRARY} )

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

   # XML Serialization Tests:
   SET( XmlSerializationTests_SRCS XmlSerializationTests.cpp )

   QT4_AUTOMOC( ${XmlSerializationTests_SRCS} )

   ADD_EXECUTABLE( XmlSerializationTests ${XmlSerializationTests_SRCS} )

   TARGET_LINK_LIBRARIES( XmlSerializationTests ${QT_QTTEST_LIBRARY} )
   TARGET_LINK_LIBRARIES( XmlSerializationTests ${QT_QTXML_LIBRARY} )
   TARGET_LINK_LIBRARIES( XmlSerializationTests ${QT_LIBRARIES} )

   # now add the tests:
   ADD_TEST( SqLiteStorageTests ${EXECUTABLE_OUTPUT_PATH}/SqLiteStorageTests )
   ADD_TEST( ControllerTests ${EXECUTABLE_OUTPUT_PATH}/ControllerTests )
   ADD_TEST( CharmDataModelTests ${EXECUTABLE_OUTPUT_PATH}/CharmDataModelTests )
   ADD_TEST( BackendIntegrationTests ${EXECUTABLE_OUTPUT_PATH}/BackendIntegrationTests )
   ADD_TEST( XmlSerializationTests ${EXECUTABLE_OUTPUT_PATH}/XmlSerializationTests )

ENDIF ( QT4_FOUND )

IF ( NOT QT4_FOUND )
   MESSAGE( "Fatal: Qt 4 not found." )
ENDIF ( NOT QT4_FOUND) 

