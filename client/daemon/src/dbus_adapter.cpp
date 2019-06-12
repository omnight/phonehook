#include <QDebug>
#include <QtDBus>
#include <QJsonObject>

#include "dbus_adapter.h"
#include "lookup_thread.h"
#include "dbus.h"

dbus_adapter *dbus_adapter::m_Instance = NULL;

dbus_adapter::dbus_adapter(QObject *parent) :
    QDBusAbstractAdaptor(parent)
{
    lookupState = "init_state";

//    qDebug() << "test connect close" <<
//    QDBusConnection::sessionBus().connect("",
//        "/org/freedesktop/Notifications",
//        "org.freedesktop.Notifications",
//        "NotificationClosed",
//        this, SLOT(notificationClosed(uint,uint)) );
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

    // send state to interface listeners
    QMetaObject::invokeMethod(dbus_adapter::Instance(),
                              "searchState", Qt::QueuedConnection,
                              Q_ARG(QString,state));
}

void dbus_adapter::sendQml(QString qml) {

        qDebug() << "sending qml...";
        messageToPopup("result", qml);
}

void dbus_adapter::testLookup(QString number, int botid) {

    phonenumber pn(number,
                   phonenumber::mobilecc_to_iso32662( dbus::Instance()->mobileCountryCode() ),
                   QString::number( dbus::Instance()->mobileNetworkCode()  ));

    qDebug() << pn.number_local << pn.number_international;

    dbus_adapter::Instance()->set_lookupState("activate:lookup");
    lookup_thread *lt = new lookup_thread();

    QList<int> bots;
    if(botid != 0) bots << botid;
    lt->start(pn, bots);

}

void dbus_adapter::testLookup2(QString number) {
    // test number with all restrictions in place
    dbus::Instance()->processNumber(number,true);
}




void dbus_adapter::messageToPopup(QString function, QString data) {

    QMetaObject::invokeMethod(this, "command", Qt::QueuedConnection, Q_ARG(QString, function), Q_ARG(QString, data));
//    emit command(function,data);

}


void dbus_adapter::search(QVariantMap parameters, QList<QVariant> bots) {

    qDebug() << "hello!!" << parameters << bots;

    QMap<QString,QString> params;
    for(auto a = parameters.begin(); a != parameters.end(); ++a ) {
        params.insert(a.key(), a.value().toString());
    }

    QList<int> botInt;
    foreach(auto v, bots) {
        botInt.append(v.toInt());
    }

    qDebug() << "search params" << params << "bots" << botInt;



    dbus_adapter::Instance()->set_lookupState("activate:" + params["tagWanted"]);

    lookup_thread *lt = new lookup_thread();
    lt->start(params, botInt);

}

void dbus_adapter::login(int bot_id, QVariantMap parameters) {
    QMap<QString,QString> params;
    for(auto a = parameters.begin(); a != parameters.end(); ++a ) {
        params.insert(a.key(), a.value().toString());
    }

    lookup_thread *lt = new lookup_thread();
    lt->start(params, QList<int>() << bot_id);

}


void dbus_adapter::blockLastCall(QString alias) {
    dbus::Instance()->blockLastCall(alias);
}
