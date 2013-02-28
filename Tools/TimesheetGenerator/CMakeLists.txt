INCLUDE_DIRECTORIES( ${Charm_SOURCE_DIR} ${Charm_BINARY_DIR} )

SET(
    TimesheetGenerator_SRCS
    main.cpp
    Options.cpp
)

ADD_EXECUTABLE( TimesheetGenerator ${TimesheetGenerator_SRCS} )

TARGET_LINK_LIBRARIES( TimesheetGenerator CharmCore ${QT_LIBRARIES} )

INSTALL( TARGETS TimesheetGenerator DESTINATION ${BIN_INSTALL_DIR} )
