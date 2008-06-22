IF ( QT4_FOUND )
   INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} )
   INCLUDE_DIRECTORIES( ${QT_QTCORE_INCLUDE_DIR} )
   INCLUDE_DIRECTORIES( ${QT_QTSQL_INCLUDE_DIR} )
   INCLUDE_DIRECTORIES( ${QT_QTXML_INCLUDE_DIR} )
   INCLUDE_DIRECTORIES( ../.. ) # Charm root
   include (${QT_USE_FILE})

   LINK_LIBRARIES( ${QT_QTCORE_LIBRARY} )
   LINK_LIBRARIES( ${QT_QTXML_LIBRARY} )
   LINK_LIBRARIES( ${QT_QTSQL_LIBRARY} )
   
   IF (CMAKE_COMPILER_IS_GNUCXX)
      ADD_DEFINITIONS( -Wall -Wundef -Wcast-align -Wconversion -Wchar-subscripts -Wpointer-arith 
		       -Wwrite-strings -Wpacked -Wformat-security -Wmissing-format-attribute 
		       -Wold-style-cast )
# to be added:       
#	-Woverloaded-virtual  -Wshadow 
#  -Wnon-virtual-dtor # Qt has too many of those already
   ENDIF (CMAKE_COMPILER_IS_GNUCXX)

    SET( TimesheetProcessor_SRCS main.cpp 
    		CommandLine.cpp 
    		Operations.cpp
    		Database.cpp )

    QT4_AUTOMOC ( ${TimesheetProcessor_SRCS} )

    ADD_EXECUTABLE( TimesheetProcessor ${TimesheetProcessor_SRCS} )
    TARGET_LINK_LIBRARIES( TimesheetProcessor CharmCore )
    TARGET_LINK_LIBRARIES( TimesheetProcessor ${QT_LIBRARIES} )
#    INSTALL( TARGETS TimesheetProcessor DESTINATION bin )

ENDIF ( QT4_FOUND )

IF ( NOT QT4_FOUND )
   MESSAGE( "Fatal: Qt 4 not found." )
ENDIF ( NOT QT4_FOUND) 

