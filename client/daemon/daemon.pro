# The name of your application
TARGET = phonehook-daemon


debug {
    QMAKE_CXXFLAGS_RELEASE -= -O
    QMAKE_CXXFLAGS_RELEASE -= -O1
    QMAKE_CXXFLAGS_RELEASE -= -O2
    QMAKE_CXXFLAGS_RELEASE -= -O3

    # add the desired -O3 if not present
    QMAKE_CXXFLAGS_RELEASE += -O3
}

QT += dbus xml sql gui-private xmlpatterns
CONFIG += sailfishapp c++17
SOURCES += src/ph-daemon.cpp \
    src/robot_base.cpp \
    src/process_data.cpp \
    src/lookup_thread.cpp \
    src/handler_url.cpp \
    src/handler_regexp.cpp \
    src/handler_format.cpp \
    src/entities.cpp \
    src/dbus_adapter.cpp \
    src/dbus.cpp \
    src/db.cpp \
    src/phonenumber.cpp \
    src/quicksand.cpp \
    src/compression.cpp \
    src/overlay.cpp \
    src/blocking.cpp \
    src/auto_update.cpp \    
    ../common/bot_download.cpp \
    ../common/util.cpp \
    ../common/setting.cpp


service.files = phonehook-daemon.service
service.path = /usr/lib/systemd/user

INSTALLS = service
LIBS += -lz

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n
TRANSLATIONS += translations/phonehook-daemon-es.ts \
                translations/phonehook-daemon-sv.ts \
                translations/phonehook-daemon-ru.ts \
                translations/phonehook-daemon-fi.ts \
                translations/phonehook-daemon-de.ts


INCLUDEPATH += ../common

HEADERS += \
    src/robot_base.h \
    src/process_data.h \
    src/lookup_thread.h \
    src/handler_url.h \
    src/handler_regexp.h \
    src/handler_format.h \
    src/entities.h \
    src/dbus_adapter.h \
    src/dbus.h \
    src/db.h \
    src/phonenumber.h \
    src/quicksand.h \
    src/compression.h \
    src/overlay.h \
    src/blocking.h \
    src/auto_update.h \
    ../common/macros.h \
    ../common/util.h \
    ../common/bot_download.h \
    ../common/setting.h

RESOURCES += \
    ph.qrc

OTHER_FILES += \
    qml/popup/gui.qml \
    qml/popup/Hex.qml \
    qml/popup/HexGrid.qml \
    translations/*.ts


