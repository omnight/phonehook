# NOTICE:
#
# Application name defined in TARGET has a corresponding QML filename.
# If name defined in TARGET is changed, the following needs to be done
# to match new name:
#   - corresponding QML filename must be changed
#   - desktop icon filename must be changed
#   - desktop filename must be changed
#   - icon definition filename in desktop file must be changed
#   - translation filenames have to be changed

# The name of your application
TARGET = phonehook

CONFIG += sailfishapp
QT += dbus xml sql xmlpatterns


LIBS += -L$${PWD}/lib -lnemonotifications-qt5

SOURCES += \
    src/phonehook.cpp \
    src/bots.cpp \
    src/countries.cpp

OTHER_FILES += qml/phonehook.qml \
    qml/cover/CoverPage.qml \
    qml/pages/FirstPage.qml \
    rpm/phonehook.changes.in \
    rpm/phonehook.spec \
    rpm/phonehook.yaml \
    translations/*.ts \
    phonehook.desktop \
    qml/pages/PageServerBotList.qml \
    qml/pages/PageBotDetails.qml \
    qml/setting/SettingString.qml \
    qml/setting/SettingBool.qml \
    qml/pages/PageBotTest.qml \
    qml/pages/PageAppSettings.qml \
    qml/popup/gui.qml \
    qml/popup/images/ph-logo.png \
    qml/pages/PageBotDownload.qml \
    qml/pages/PageDownloadWait.qml \
    qml/images/new-48.png \
    qml/images/approval-48.png \
    qml/setting/SettingPassword.qml \
    qml/popup/gui2.qml \
    qml/pages/PageSearchStart.qml \
    qml/pages/PageSearchResults.qml

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n
TRANSLATIONS += translations/phonehook-de.ts

HEADERS += \
    src/notification.h \
    src/notification.h \
    src/bots.h \
    src/countries.h

RESOURCES +=

