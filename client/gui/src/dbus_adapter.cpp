#include <QDebug>
#include "dbus_adapter.h"
#include <QtDBus>

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

void dbus_adapter::ready() {

    qDebug() << "READY!!";
    m_bIsReady = true;

    for(const QString &q:qmlItems) {
        sendQml(q);
    }

    QMetaObject::invokeMethod(this, "lookupStateChanged", Qt::QueuedConnection, Q_ARG(QString, lookupState));
    qmlItems.clear();
}

void dbus_adapter::getLookupState() {
    qDebug() << "getLookupState()";
    QMetaObject::invokeMethod(this, "lookupStateChanged", Qt::QueuedConnection, Q_ARG(QString, lookupState));
}

void dbus_adapter::set_lookupState(QString state) {
    qDebug() << "setting state = " << state;
    lookupState = state;
    QMetaObject::invokeMethod(this, "lookupStateChanged", Qt::QueuedConnection, Q_ARG(QString, lookupState));
}

void dbus_adapter::sendQml(QString qml) {

//    if(!m_bIsReady) {
//        qmlItems.append(qml);
//    } else {
        qDebug() << "sending qml...";
        QMetaObject::invokeMethod(this, "qmlResult", Qt::QueuedConnection, Q_ARG(QString, qml));
//    }
}

void dbus_adapter::clearReady() {
    m_bIsReady = false;
}
