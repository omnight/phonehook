#include <QDebug>
#include "dbus_adapter.h"
#include <QtDBus>
#include "lookup_thread.h"

dbus_adapter *dbus_adapter::m_Instance = NULL;

dbus_adapter::dbus_adapter(QObject *parent) :
    QDBusAbstractAdaptor(parent)
{
    lookupState = "init_state";
}


dbus_adapter* dbus_adapter::Instance(QObject *parent) {
    if(m_Instance == NULL) {
        qDebug() << "new dbus_adapter";
        m_Instance = new dbus_adapter(parent);
    }

    return m_Instance;
}


void dbus_adapter::Hello() {
    qDebug() << "HELLO FROM DIALER!!";
    set_lookupState("banan");
}

void dbus_adapter::getLookupState() {
    qDebug() << "getLookupState()";
    messageToPopup("stateChange", lookupState);
}

void dbus_adapter::set_lookupState(QString state) {
    qDebug() << "setting state = " << state;
    lookupState = state;
    messageToPopup("stateChange", lookupState);
}

void dbus_adapter::sendQml(QString qml) {

        qDebug() << "sending qml...";
        messageToPopup("result", qml);
}

void dbus_adapter::testLookup(QString number, int botid) {
    dbus_adapter::Instance()->set_lookupState("activate");
    lookup_thread *lt = new lookup_thread();
    lt->start(number, botid);
}

void dbus_adapter::messageToPopup(QString function, QString data) {
    QDBusMessage pcall =
    QDBusMessage::createMethodCall("com.omnight.lipstick",
                                   "/",
                                   "com.jolla.lipstick.ConnectionSelectorIf",
                                   "command");

    QVariantList args;
    args.append(function);
    args.append(data);
    pcall.setArguments(args);

    qDebug() << "dbus popup method call " << function << data <<
    QDBusConnection::sessionBus().send(pcall);
}
