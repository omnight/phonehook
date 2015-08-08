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

QT += dbus xml sql gui-private
CONFIG += sailfishapp c++11
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
    src/setting.cpp

service.files = phonehook-daemon.service
service.path = /usr/lib/systemd/user

INSTALLS = service
LIBS += -lz

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n
TRANSLATIONS += translations/ph-daemon-de.ts

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
    src/setting.h

RESOURCES += \
    ph.qrc

OTHER_FILES += \
    qml/popup/gui.qml \
    qml/popup/Hex.qml \
    qml/popup/HexGrid.qml
