#include <QDebug>
#include "dbus_adapter.h"
#include <QtDBus>
#include "lookup_thread.h"
#include "dbus.h"
#include <QJsonObject>

dbus_adapter *dbus_adapter::m_Instance = NULL;

dbus_adapter::dbus_adapter(QObject *parent) :
    QDBusAbstractAdaptor(parent)
{
    lookupState = "init_state";

    compabilityNotificationTimer.setInterval(3500);
    compabilityNotificationTimer.setSingleShot(true);
    connect(&compabilityNotificationTimer, SIGNAL(timeout()), this, SLOT(compabilityNotificationTimerTick()));

    m_compabilityNotificationId = 0;

    qDebug() << "test connect close" <<
    QDBusConnection::sessionBus().connect("",
        "/org/freedesktop/Notifications",
        "org.freedesktop.Notifications",
        "NotificationClosed",
        this, SLOT(notificationClosed(uint,uint)) );
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
    lt->start(pn, QList<int>() << botid);
}


void dbus_adapter::compabilityNotificationTimerTick() {

    if(m_compabilityNotificationId) {

        QDBusMessage closeCall =
        QDBusMessage::createMethodCall("org.freedesktop.Notifications",
                                       "/org/freedesktop/Notifications",
                                       "org.freedesktop.Notifications",
                                       "CloseNotification");

        closeCall << m_compabilityNotificationId;
        QDBusConnection::sessionBus().send(closeCall);
    }

    if(compabilityNotificationQueue.count() == 0) return;
    compabilityNotificationTimer.start();

    QPair<QString,QString> msg = compabilityNotificationQueue.dequeue();

    QDBusMessage pcall =
    QDBusMessage::createMethodCall("org.freedesktop.Notifications",
                                   "/org/freedesktop/Notifications",
                                   "org.freedesktop.Notifications",
                                   "Notify");


    pcall << "phonehook"
          << uint(0)
          << "icon-lock-information"
          << msg.first
          << msg.second
          << QStringList { "default", "" }
          << QVariantMap {{ "x-nemo-preview-summary", msg.first},
                          { "x-nemo-preview-body", msg.second},
                          { "x-nemo-remote-action-default", "com.omnight.phonehook / com.omnight.phonehook notificationAction"  }}
          << 2000;



    QDBusConnection::sessionBus().callWithCallback(pcall, this, SLOT(notificationCallback(uint)));

}

void dbus_adapter::notificationCallback(uint code) {
    m_compabilityNotificationId = code;
    qDebug() << code;

}

void dbus_adapter::notificationAction() {
    qDebug() << "notification action!";
    compabilityNotificationQueue.clear();
}

void dbus_adapter::notificationClosed(uint id, uint uh) {
    if(id == m_compabilityNotificationId) {
        qDebug() << "close app notification" << id << uh;
        m_compabilityNotificationId = 0;
    }
}

void dbus_adapter::compabilityNotification(QString jsonData) {
    QJsonDocument jd = QJsonDocument::fromJson(jsonData.toUtf8());

    compabilityNotificationQueue.clear();

    if(jd.isArray()) {

        int duration = 0;

        for(int i=0; i < jd.array().count(); i++) {
            QString line = jd.array().at(i).toObject().value("title").toString() + ": " + jd.array().at(i).toObject().value("value").toString();
            compabilityNotificationFields.append(line);
        }

        if(compabilityNotificationFields.count() == 0) return;

        while(duration < 30000) {
            for(int i=0; i < compabilityNotificationFields.count(); i+=2) {
                QString line1,line2;
                line1 = compabilityNotificationFields[i];
                if(compabilityNotificationFields.count() > i+1)
                    line2 = compabilityNotificationFields[i+1];

                compabilityNotificationQueue.append( QPair<QString,QString>(line1,
                                                                            line2) );
                duration += compabilityNotificationTimer.interval();
            }
        }

        if(!compabilityNotificationTimer.isActive())
            compabilityNotificationTimerTick();
    }

}

void dbus_adapter::messageToPopup(QString function, QString data) {

    if(!dbus::Instance()->compabilityMode()) {

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

    } else {
        // compability mode!

        if(function == "activate:lookup")
            compabilityNotificationFields.clear();

        if(function == "result") {
            compabilityNotification(data);
        }
    }

//  option 2 (non working) - interface signal
//    emit command(function,data);

}


void dbus_adapter::search(QMap<QString,QVariant> parameters, QList<QVariant> bots) {

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
