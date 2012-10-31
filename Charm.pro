
QT += core gui xml sql network widgets
INCLUDEPATH += Core/
INCLUDEPATH += Charm/

TARGET = AndCharm
TEMPLATE = app
RESOURCES = Charm/CharmResources.qrc

DEFINES += 'CHARM_VERSION=\'\"0.1a\"\''

SOURCES += $$files(Core/*.cpp)
SOURCES += $$files(Charm/*.cpp)
SOURCES += $$files(Charm/Commands/*.cpp)
SOURCES += $$files(Charm/HttpClient/*.cpp)
SOURCES += $$files(Charm/Icons/*.cpp)
SOURCES += $$files(Charm/Idle/*.cpp)
SOURCES += $$files(Charm/Reports/*.cpp)
SOURCES += $$files(Charm/TimeTrackingView/*.cpp)

SOURCES -= $$files(Charm/IdleWidget.cpp)
SOURCES -= $$files(Charm/Idle/Windows*.cpp)
SOURCES -= $$files(Charm/Idle/X11*.cpp)
SOURCES -= $$files(Charm/keychain_dbus.cpp)
SOURCES -= $$files(Charm/keychain_mac.cpp)

HEADERS += $$files(Core/*.h)
HEADERS += $$files(Charm/*.h)
HEADERS += $$files(Charm/Commands/*.h)
HEADERS += $$files(Charm/HttpClient/*.h)
HEADERS += $$files(Charm/Icons/*.h)
HEADERS += $$files(Charm/Idle/*.h)
HEADERS += $$files(Charm/Reports/*.h)
HEADERS += $$files(Charm/TimeTrackingView/*.h)

HEADERS -= $$files(Charm/MacApplication.h)
HEADERS -= $$files(Charm/IdleWidget.h)
HEADERS -= $$files(Charm/Idle/Windows*.h)
HEADERS -= $$files(Charm/Idle/X11*.h)
HEADERS -= $$files(Charm/keychain_dbus.h)
HEADERS -= $$files(Charm/keychain_dbus_p.h)
HEADERS -= $$files(Charm/keychain_mac.h)
HEADERS -= $$files(Charm/keychain_win.h)

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

OTHER_FILES += \
    android/version.xml \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/src/org/kde/necessitas/origo/QtActivity.java \
    android/src/org/kde/necessitas/origo/QtApplication.java \
    android/res/values-et/strings.xml \
    android/res/values-fr/strings.xml \
    android/res/values-fa/strings.xml \
    android/res/drawable-hdpi/icon.png \
    android/res/values-nb/strings.xml \
    android/res/drawable/icon.png \
    android/res/drawable/logo.png \
    android/res/values-ru/strings.xml \
    android/res/values-id/strings.xml \
    android/res/values-zh-rCN/strings.xml \
    android/res/values-zh-rTW/strings.xml \
    android/res/values-ro/strings.xml \
    android/res/values/strings.xml \
    android/res/values/libs.xml \
    android/res/values-de/strings.xml \
    android/res/values-ms/strings.xml \
    android/res/drawable-mdpi/icon.png \
    android/res/values-pl/strings.xml \
    android/res/values-es/strings.xml \
    android/res/values-nl/strings.xml \
    android/res/layout/splash.xml \
    android/res/values-el/strings.xml \
    android/res/drawable-ldpi/icon.png \
    android/res/values-it/strings.xml \
    android/res/values-ja/strings.xml \
    android/res/values-pt-rBR/strings.xml \
    android/res/values-rs/strings.xml \
    android/AndroidManifest.xml \
    android/res/layout/splash.xml \
    android/res/values/strings.xml \
    android/res/values-de/strings.xml \
    android/res/values-el/strings.xml \
    android/res/values-es/strings.xml \
    android/res/values-et/strings.xml \
    android/res/values-fa/strings.xml \
    android/res/values-fr/strings.xml \
    android/res/values-id/strings.xml \
    android/res/values-it/strings.xml \
    android/res/values-ja/strings.xml \
    android/res/values-ms/strings.xml \
    android/res/values-nb/strings.xml \
    android/res/values-nl/strings.xml \
    android/res/values-pl/strings.xml \
    android/res/values-pt-rBR/strings.xml \
    android/res/values-ro/strings.xml \
    android/res/values-rs/strings.xml \
    android/res/values-ru/strings.xml \
    android/res/values-zh-rCN/strings.xml \
    android/res/values-zh-rTW/strings.xml \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/src/org/kde/necessitas/origo/QtActivity.java \
    android/src/org/kde/necessitas/origo/QtApplication.java \
    android/version.xml \
    android/AndroidManifest.xml \
    android/res/drawable/icon.png \
    android/res/drawable/logo.png \
    android/res/drawable-hdpi/icon.png \
    android/res/drawable-ldpi/icon.png \
    android/res/drawable-mdpi/icon.png \
    android/res/values/libs.xml \
    android/version.xml \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/src/org/kde/necessitas/origo/QtActivity.java \
    android/src/org/kde/necessitas/origo/QtApplication.java \
    android/res/values-et/strings.xml \
    android/res/values-fr/strings.xml \
    android/res/values-fa/strings.xml \
    android/res/drawable-hdpi/icon.png \
    android/res/values-nb/strings.xml \
    android/res/drawable/icon.png \
    android/res/drawable/logo.png \
    android/res/values-ru/strings.xml \
    android/res/values-id/strings.xml \
    android/res/values-zh-rCN/strings.xml \
    android/res/values-zh-rTW/strings.xml \
    android/res/values-ro/strings.xml \
    android/res/values/strings.xml \
    android/res/values/libs.xml \
    android/res/values-de/strings.xml \
    android/res/values-ms/strings.xml \
    android/res/drawable-mdpi/icon.png \
    android/res/values-pl/strings.xml \
    android/res/values-es/strings.xml \
    android/res/values-nl/strings.xml \
    android/res/layout/splash.xml \
    android/res/values-el/strings.xml \
    android/res/drawable-ldpi/icon.png \
    android/res/values-it/strings.xml \
    android/res/values-ja/strings.xml \
    android/res/values-pt-rBR/strings.xml \
    android/res/values-rs/strings.xml \
    android/AndroidManifest.xml

