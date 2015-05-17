#include "blocking.h"
#include <QtDBus>
#include "db.h"
#include <QSqlQuery>
#include "setting.h"

blocking* blocking::m_Instance = NULL;

blocking::blocking(QObject *parent) :
    QObject(parent)
{
    m_Instance = this;




}


blocking* blocking::Instance() {
    if(m_Instance == NULL)
        return new blocking();
}


bool blocking::checkManualBlock(phonenumber number) {

    QSqlQuery checkManualBlock;

    checkManualBlock.prepare("SELECT * FROM block WHERE (? like REPLACE(number, '*','%')) OR (? like REPLACE(number, '*','%'));");
    checkManualBlock.addBindValue(number.number_local);
    checkManualBlock.addBindValue(number.number_international);

    checkManualBlock.exec();

    if(checkManualBlock.next()) {
        QSqlQuery addBlockHistory("INSERT INTO block_history(block_id) \
                                  VALUES(?)");
        addBlockHistory.addBindValue(checkManualBlock.value("id").toInt());
        addBlockHistory.exec();
        return true;
    }

    return false;

}


bool blocking::checkContactBlock(phonenumber number) {

    QSqlQuery blockedContactsQuery("SELECT contact_id FROM block WHERE type=1");

    blockedContactsQuery.exec();

    QStringList blockedIds;

    while(blockedContactsQuery.next()) {
        blockedIds.append(
            blockedContactsQuery.value("contact_id").toString()
        );
    }

    // no blocked ids?
    if(blockedIds.count() == 0)
        return false;

    QString allIds = blockedIds.join(",");

    QSqlQuery cq(db::Instance()->getContactsDb() );


    cq.prepare("SELECT ContactId FROM phonenumbers WHERE \
               (phonenumber = ? or phonenumber = ?) AND \
               ContactId IN (" + allIds + ")");

    cq.addBindValue(number.number_international);
    cq.addBindValue(number.number_local);

    cq.exec();

    if(cq.next()) {
        QSqlQuery addBlockHistory("INSERT INTO block_history(block_id) \
                                  SELECT id FROM block WHERE contact_id=?");
        addBlockHistory.addBindValue(cq.value("ContactId").toInt());
        addBlockHistory.exec();

        qDebug() << "blocked! contact id = " << cq.value("ContactId").toInt();

        return true;
    }

    return false;
}

bool blocking::preCheckBlock(phonenumber number) {
    if(checkManualBlock(number) || checkContactBlock(number)) {
        hangup();
        return true;
    }

    return false;
}

void blocking::hangup() {
    //dbus-send --system --type=method_call --print-reply --dest=org.ofono /ril_0 org.ofono.VoiceCallManager.HangupAll

    // method 1:

//    QDBusMessage m = QDBusMessage::createMethodCall("org.ofono",
//                                                    "/ril_0",
//                                                    "org.ofono.VoiceCallManager",
//                                                    "HangupAll");
//    QDBusConnection::systemBus().call(m);


    // method 2:

//    QProcess p;
//    p.startDetached("killall", QStringList() << "voicecall-ui");

//    QStringList args;
//    p.startDetached("killall", QStringList() << "voicecall-manager");


    // method 3:

    QDBusMessage m = QDBusMessage::createMethodCall("org.nemomobile.voicecall",
                                                    "/calls/active",
                                                    "org.nemomobile.voicecall.VoiceCall",
                                                    "silenceRingtone");
    QDBusConnection::sessionBus().call(m);

    m = QDBusMessage::createMethodCall("org.nemomobile.voicecall",
                                                    "/calls/active",
                                                    "org.nemomobile.voicecall.VoiceCall",
                                                    "hangup");
    QDBusConnection::sessionBus().call(m);


    if(setting::get("kill_voicecall_ui", "false") == "true") {
        QProcess p;
        p.startDetached("killall", QStringList() << "voicecall-ui");
    }


}
