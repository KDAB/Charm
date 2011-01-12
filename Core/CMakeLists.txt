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
        XmlSerialization.cpp
)
QT4_AUTOMOC( ${CharmCore_SRCS} )
ADD_LIBRARY( CharmCore STATIC ${CharmCore_SRCS} )
