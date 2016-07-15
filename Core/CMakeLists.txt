if(POLICY CMP0020)
    CMAKE_POLICY(SET CMP0020 NEW)
endif()

SET(
    CharmCore_SRCS
    CharmConstants.cpp
    CharmExceptions.cpp
    Controller.cpp
    Dates.cpp
    SqlRaiiTransactor.cpp
    SqLiteStorage.cpp
    MySqlStorage.cpp
    Configuration.cpp
    SqlStorage.cpp
    Event.cpp
    Task.cpp
    TaskListMerger.cpp
    State.cpp
    CharmDataModel.cpp
    TaskTreeItem.cpp
    TimeSpans.cpp
    CharmCommand.cpp
    SmartNameCache.cpp
    XmlSerialization.cpp
)

ADD_LIBRARY( CharmCore STATIC ${CharmCore_SRCS} )

kde_target_enable_exceptions( CharmCore PUBLIC )

TARGET_LINK_LIBRARIES( CharmCore Qt5::Core Qt5::Widgets Qt5::Sql Qt5::Xml)
