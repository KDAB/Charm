IF ( QT4_FOUND )
   INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} )
   INCLUDE_DIRECTORIES( ${QT_QTCORE_INCLUDE_DIR} )
   INCLUDE_DIRECTORIES( ${QT_QTSQL_INCLUDE_DIR} )
   INCLUDE_DIRECTORIES( ${QT_QTXML_INCLUDE_DIR} )

   LINK_LIBRARIES( ${QT_QTCORE_LIBRARY} )
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
		       CharmConstants.cpp
		       Controller.cpp
		       SqLiteStorage.cpp
               MySqlStorage.cpp
		       Configuration.cpp
		       SqlStorage.cpp
		       Event.cpp
		       Task.cpp
		       State.cpp
		       CharmDataModel.cpp
		       TaskTreeItem.cpp 
		       TimeSpans.cpp
		       CharmCommand.cpp
		       XmlSerialization.cpp
)
    
    QT4_AUTOMOC ( ${CharmCore_SRCS} )

    ADD_LIBRARY( CharmCore STATIC ${CharmCore_SRCS} )

ENDIF ( QT4_FOUND )

IF ( NOT QT4_FOUND )
   MESSAGE( "Fatal: Qt 4 not found." )
ENDIF ( NOT QT4_FOUND) 

