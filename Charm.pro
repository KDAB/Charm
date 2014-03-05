!android: error("Building Charm with QMake is not supported, and used only for Qt/Android experiments. For everything else, please use the CMake build system.")

QT += core gui xml sql network widgets
INCLUDEPATH += Core/
INCLUDEPATH += Charm/

TARGET = AndCharm
TEMPLATE = app
RESOURCES = Charm/CharmResources.qrc

DEFINES += 'CHARM_VERSION=\'\"0.1a\"\''
DEFINES += 'CHARM_IDLE_TIME=0'
DEFINES += QT_NO_DBUS QT_NO_PRINTER

SOURCES += $$files(Core/*.cpp)
SOURCES += $$files(Charm/*.cpp)
SOURCES += $$files(Charm/Commands/*.cpp)
SOURCES += $$files(Charm/HttpClient/*.cpp)
SOURCES += $$files(Charm/Icons/*.cpp)
SOURCES += $$files(Charm/Idle/*.cpp)
SOURCES += $$files(Charm/Reports/*.cpp)
SOURCES += $$files(Charm/TimeTrackingView/*.cpp)
SOURCES += $$files(Charm/Qocoa/*_nonmac.cpp)

SOURCES -= $$files(Charm/IdleWidget.cpp)
SOURCES -= $$files(Charm/Idle/Windows*.cpp)
SOURCES -= $$files(Charm/Idle/X11*.cpp)
SOURCES -= $$files(Charm/keychain_dbus.cpp)
SOURCES -= $$files(Charm/keychain_mac.cpp)
SOURCES -= $$files(Charm/keychain_win.cpp)

HEADERS += $$files(Core/*.h)
HEADERS += $$files(Charm/*.h)
HEADERS += $$files(Charm/Commands/*.h)
HEADERS += $$files(Charm/HttpClient/*.h)
HEADERS += $$files(Charm/Icons/*.h)
HEADERS += $$files(Charm/Idle/*.h)
HEADERS += $$files(Charm/Reports/*.h)
HEADERS += $$files(Charm/TimeTrackingView/*.h)
HEADERS += $$files(Charm/Qocoa/*.h)

HEADERS -= $$files(Charm/MacApplication.h)
HEADERS -= $$files(Charm/IdleWidget.h)
HEADERS -= $$files(Charm/Idle/Windows*.h)
HEADERS -= $$files(Charm/Idle/X11*.h)
HEADERS -= $$files(Charm/keychain_dbus.h)
HEADERS -= $$files(Charm/keychain_dbus_p.h)
HEADERS -= $$files(Charm/keychain_mac.h)
HEADERS -= $$files(Charm/keychain_win.h)
HEADERS -= $$files(Charm/OpeningScene.h)

FORMS += $$files(Core/*.ui)
FORMS += $$files(Charm/*.ui)
FORMS += $$files(Charm/Commands/*.ui)
FORMS += $$files(Charm/HttpClient/*.ui)
FORMS += $$files(Charm/Icons/*.ui)
FORMS += $$files(Charm/Idle/*.ui)
FORMS += $$files(Charm/Reports/*.ui)
FORMS += $$files(Charm/TimeTrackingView/*.ui)

CONFIG += mobility
MOBILITY =

# Disable some of the noise for now.
*-g++*|*-clang*|*-llvm* {
    QMAKE_CXXFLAGS += -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function
}

# CMake works with include files named "filename.moc" while qmake expects
# "moc_filename.cpp". Since qmake does not provide any way to change that
# to the requirement we just hack around.

#new_moc.output = ${QMAKE_FILE_BASE}.moc
#new_moc.commands = moc -i -nw ${QMAKE_FILE_NAME} -o ${QMAKE_FILE_OUT}
#new_moc.depend_command = $$QMAKE_CXX -E -M ${QMAKE_FILE_NAME} | sed "s/^.*: //"
#new_moc.input = HEADERS
#QMAKE_EXTRA_COMPILERS += new_moc

MOC_HEADERS = $$HEADERS
#MOC_HEADERS -= $$files(Charm/keychain_p.h)
for(hdr, MOC_HEADERS) {
    fdir=$$dirname(hdr)
    base=$$basename(hdr)
    fname=$$section(base, ".", 0, 0)
    in=$${LITERAL_HASH}include <moc_$${fname}.cpp>
    out=$${OUT_PWD}/$${fname}.moc
    exists( $${fdir}/$${fname}.cpp ) {
        system(echo \"$$in\" > \"$$out\")
    }
}

# Create that CharmCMake.h file that is auto-created by cmake ahnd included
# everywhere. Here we could also hard-code defines or whatever that file
# includes when cmake creates it.
system('echo "" > "$${OUT_PWD}/CharmCMake.h"')
