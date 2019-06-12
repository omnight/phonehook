#include <QtDBus>
#include <QSqlQuery>
#include <QSqlError>

#include "blocking.h"
#include "db.h"
#include "setting.h"


SINGLETON_CPP(blocking)

blocking::blocking(QObject *parent) :
    QObject(parent)
{

}

bool blocking::checkManualBlock(phonenumber number) {

    QSqlQuery checkManualBlock;

    checkManualBlock.prepare(R"(
        SELECT * FROM block
        WHERE (? like REPLACE(number, '*','%'))
           OR (? like REPLACE(number, '*','%'));
    )");


    checkManualBlock.addBindValue(number.number_local);
    checkManualBlock.addBindValue(number.number_international);

    checkManualBlock.exec();

    if(checkManualBlock.next()) {
        QSqlQuery addBlockHistory(R"(
          INSERT INTO block_history(block_id,number)
          VALUES(?,?)
        )");
        addBlockHistory.addBindValue(checkManualBlock.value("id").toInt());
        addBlockHistory.addBindValue(number.number_international);
        addBlockHistory.exec();
        return true;
    }

    return false;

}


bool blocking::checkAutoBlock(phonenumber number) {
    QSqlQuery autoBlockQuery(R"(
        SELECT block.id FROM block JOIN bot_result_cache ON block.bot_id = bot_result_cache.bot_id
        WHERE bot_result_cache.telnr IN (?,?) AND bot_result_cache.block = 1
    )");

    autoBlockQuery.addBindValue(number.number_local);
    autoBlockQuery.addBindValue(number.number_international);

    if(!autoBlockQuery.exec())
        qDebug() << "check block error" << autoBlockQuery.lastError();

    if(autoBlockQuery.next()) {

        hangup();

        QSqlQuery addBlockHistory(R"(
            INSERT INTO block_history(block_id,number)
            VALUES(?,?)
        )");
        addBlockHistory.addBindValue(autoBlockQuery.value("id").toInt());
        addBlockHistory.addBindValue(""+number.number_international);
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
        QSqlQuery addBlockHistory(R"(
            INSERT INTO block_history(block_id,number)
            SELECT id,? FROM block WHERE contact_id=?
        )");
        addBlockHistory.addBindValue(number.number_international);
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

    // don't auto-check hidden
    if(number.number_local == "")
        return false;

    return checkAutoBlock(number);

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
