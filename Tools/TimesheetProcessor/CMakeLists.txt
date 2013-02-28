INCLUDE_DIRECTORIES( ${Charm_SOURCE_DIR} ${Charm_BINARY_DIR} )

SET(
    TimesheetProcessor_SRCS
    main.cpp
    CommandLine.cpp
    Operations.cpp
    Database.cpp
)

ADD_EXECUTABLE( TimesheetProcessor ${TimesheetProcessor_SRCS} )
TARGET_LINK_LIBRARIES( TimesheetProcessor CharmCore ${QT_LIBRARIES} )
INSTALL( TARGETS TimesheetProcessor DESTINATION ${BIN_INSTALL_DIR} )
