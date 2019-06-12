#include <QDebug>
#include <QSqlError>
#include <QFile>

#include "dbus.h"
#include "phonenumber.h"
#include "blocking.h"
#include "db.h"

dbus* dbus::m_instance;

dbus::dbus(QObject *parent) :
    QObject(parent)
{

    m_instance = this;

    qDebug() << "registering telepathy";

    // connect OFONO

//    qDebug() << "connect status = " <<
//                QDBusConnection::systemBus().connect("org.ofono",
//                                                      "/ril_0",
//                                    "org.ofono.VoiceCallManager",
//                                    "CallAdded",
//                                    this, SLOT(onIncomingCall(const QDBusMessage&)));


    // connect nemo.voicecall
    QDBusConnection::sessionBus().connect("",
                                          "/calls/active",
                        "org.nemomobile.voicecall.VoiceCall",
                        "lineIdChanged",
                        this, SLOT(onIncomingCall(const QDBusMessage&)));

    adapter = dbus_adapter::Instance(parent);

    m_isRoaming = false;
    m_mobileCountryCode = 0;
    m_mobileNetworkCode = 0;

    updateNetwork();


}

void dbus::updateNetwork() {

    QDBusMessage m = QDBusMessage::createMethodCall("org.ofono",
                                                    "/",
                                                    "org.ofono.Manager",
                                                    "GetModems");

    QDBusConnection::systemBus().callWithCallback(m, this, SLOT(gotModems(QDBusMessage)));
}




void dbus::gotModems(QDBusMessage reply) {

    const QDBusArgument &a = reply.arguments().at(0).value<QDBusArgument>();
    if(a.currentType() == QDBusArgument::ArrayType) {

        a.beginArray();

        while(!a.atEnd()) {
            QMap<QString, QVariant> params;
            const QDBusArgument &modem = a.asVariant().value<QDBusArgument>();

            if(modem.currentType() == QDBusArgument::StructureType) {
                modem.beginStructure();
                modem >> m_modemPath;
                modem.beginMap();
                while(!modem.atEnd()) {

                    modem.beginMapEntry();

                    QString t;
                    QVariant v;
                    modem >> t >> v;
                    params.insert(t, v);
                    modem.endMapEntry();
                }

                modem.endMap();
                modem.endStructure();

            }


            if(params["Type"].toString() == "hardware") {
                qDebug() << "found hardware modem at " << m_modemPath.path();


                QDBusMessage m = QDBusMessage::createMethodCall("org.ofono",
                                                                m_modemPath.path(),
                                                                "org.ofono.NetworkRegistration",
                                                                "GetProperties");

                QDBusConnection::systemBus().callWithCallback(m, this, SLOT(gotNetworkStatus(QDBusMessage)));

                // listen for network status changes
                QDBusConnection::systemBus().connect("org.ofono",
                                                     m_modemPath.path(),
                                                     "org.ofono.NetworkRegistration",
                                                     "PropertyChanged",
                                                     this, SLOT(gotNetworkStatusChange(QDBusMessage)));

                //break;
            }

        }
        a.endArray();
    }

}

void dbus::gotNetworkStatusChange(QDBusMessage event) {

    QString key = event.arguments().at(0).value<QString>();

    if(key == "MobileNetworkCode") {
        this->m_mobileNetworkCode = event.arguments().at(1).value<QDBusVariant>().variant().value<int>();
        qDebug() << "network changed" << this->m_mobileNetworkCode;
    }
    if(key == "MobileCountryCode")  {
        this->m_mobileCountryCode = event.arguments().at(1).value<QDBusVariant>().variant().value<int>();
        qDebug() << "country changed" << this->m_mobileCountryCode;
    }
}

void dbus::gotNetworkStatus(QDBusMessage reply) {

    m_lastNetworkUpdate = QDateTime::currentDateTimeUtc();
    const QDBusArgument &a = reply.arguments().at(0).value<QDBusArgument>();

    if(a.currentType() == QDBusArgument::MapType) {
        a.beginMap();

        while(!a.atEnd()) {
            a.beginMapEntry();
            QString t;
            QVariant v;
            a >> t >> v;

            if(t == "Status") { m_isRoaming = (v.toString() == "roaming"); }
            if(t == "MobileCountryCode") { m_mobileCountryCode = v.toInt(); }
            if(t == "MobileNetworkCode") { m_mobileNetworkCode = v.toInt(); }

            qDebug() << "network status: roaming" << m_isRoaming << "mcc" << m_mobileCountryCode << "mnc" << m_mobileNetworkCode;

            QSqlQuery qs("INSERT OR REPLACE INTO setting (key,value) VALUES(?,?);");
            qs.addBindValue("location");
            qs.addBindValue( phonenumber::mobilecc_to_iso32662(m_mobileCountryCode) );
            qs.exec();

            a.endMapEntry();
        }
        a.endMap();
    }

    qDebug() << "roaming" << m_isRoaming
             << "mobileCC" << m_mobileCountryCode
             << "mobileNC" << m_mobileNetworkCode;

}


void dbus::onIncomingCall(const QDBusMessage &a) {

    qDebug() << a;

    QString callingNr = a.arguments().at(0).toString();
    QDBusInterface voiceCallIf("org.nemomobile.voicecall",
                               "/calls/active",
                               "org.nemomobile.voicecall.VoiceCall",
                               QDBusConnection::sessionBus());

    if(!voiceCallIf.property("isIncoming").toBool()) {
        qDebug() << "ignore outgoing call";
        return;
    }

    qDebug() << "calling number = " << callingNr;


    processNumber(callingNr, false);

}

void dbus::processNumber(QString callingNr, bool isTest) {
    lastNumber = phonenumber(callingNr,
                                phonenumber::mobilecc_to_iso32662( dbus::Instance()->mobileCountryCode() ),
                                QString::number( dbus::Instance()->mobileNetworkCode() ));

    // check type of channel that has opened
    if(!isTest && blocking::Instance()->preCheckBlock(lastNumber) ) {
        return;
    }

    // don't proceed with lookups if number is hidden
    if(lastNumber.number_local == "") {
        return;
    }

    qDebug() << "check settings";

    QSqlQuery qs;
    qs.exec("SELECT key, value FROM setting");

    bool activateOnSms = false, activateOnlyUnknown = true, enableRoaming = false;
    Q_UNUSED(activateOnSms)
    while(qs.next()) {
        if(qs.value("key").toString() == "activate_only_unknown")
            activateOnlyUnknown = (qs.value("value").toString() == "true");

        if(qs.value("key").toString() == "activate_on_sms")
            activateOnSms = (qs.value("value").toString() == "true");

        if(qs.value("key").toString() == "enable_roaming")
            enableRoaming = (qs.value("value").toString() == "true");
    }

    qDebug() << "got settings only-unknown=" <<  activateOnlyUnknown << " enable_roaming=" << enableRoaming;

    if(!enableRoaming && dbus::Instance()->isRoaming()) {
        qDebug() << "disabled when roaming!";
        return ;
    }

    if(activateOnlyUnknown) {

        QSqlQuery cq(db::Instance()->getContactsDb() );

        cq.prepare("SELECT contactId FROM PhoneNumbers WHERE normalizedNumber = ?");
        cq.addBindValue(phonenumber::process(callingNr));
        if(!cq.exec()) {
            qDebug() << "error " << cq.lastError().text();
        }

        while(cq.next()) {
            qDebug() << "found existing contact with id " << cq.value("contactId").toInt();
            return ;
        }

    }

    qDebug() << "after localizing " << lastNumber.number_local;
    qDebug() << "request raise/activate";

    dbus_adapter::Instance()->set_lookupState("activate:lookup");

    lookup_thread *lt = new lookup_thread();

    qDebug() << "connecting" << lt;

    if(!isTest)
        connect(lt, &lookup_thread::gotResult, this, &dbus::lookupResult);

    lt->start(lastNumber, QList<int>());
}

void dbus::lookupResult(lookup_thread *sender) {
    if(blocking::Instance()->checkAutoBlock(lastNumber)) {
        qDebug() << "disconnecting signal" << sender;
        disconnect(sender, &lookup_thread::gotResult, this, &dbus::lookupResult);
    }
}


void dbus::blockLastCall(QString alias) {

    if(lastNumber.number_international == NULL || lastNumber.number_international == "") return ;
    if(alias == "") alias = lastNumber.number_international;

    // crop text if too long
    if(alias.length() > 23)
        alias = alias.left(20) + "...";

    QSqlQuery sq;
    sq.prepare("INSERT INTO block (type, name, number) VALUES(0,?,?)");
    sq.addBindValue(alias);
    sq.addBindValue(lastNumber.number_international);
    qDebug() << "added block! " << sq.exec();

    blocking::Instance()->hangup();
}
