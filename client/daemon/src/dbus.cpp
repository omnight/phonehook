#include "dbus.h"
#include <QDebug>
#include <QSqlError>
#include "lookup_thread.h"
#include "phonenumber.h"
#include <QFile>

dbus* dbus::m_instance;

dbus::dbus(QObject *parent) :
    QObject(parent)
{

    m_instance = this;

    qDebug() << "registering telepathy";
    qDebug() << "connect status = " <<
                QDBusConnection::sessionBus().connect("",
                                                      "/org/freedesktop/Telepathy/Connection/ring/tel/ring", ///ring/tel/ring
                                    "org.freedesktop.Telepathy.Connection.Interface.Requests",
                                    "NewChannels",
                                    this, SLOT(onIncomingCall(const QDBusMessage&)));

    adapter = dbus_adapter::Instance(parent);

    m_isRoaming = false;
    m_mobileCountryCode = 0;
    m_mobileNetworkCode = 0;

    updateNetwork();

    QFile lipstick;
    lipstick.setFileName("/usr/share/lipstick-jolla-home-qt5/compositor.qml");

    if(lipstick.exists()) {
        lipstick.open(QFile::ReadOnly);
        QString lipstickText = lipstick.readAll();
        lipstick.close();

        m_bHomescreenPatched = lipstickText.contains("inject");
    } else {
        m_bHomescreenPatched = false;
    }

    QSqlQuery qs;
    qs.exec("SELECT key, value FROM setting WHERE key='compability_mode'");
    m_bCompabilityMode = false;

    while(qs.next()) {
        m_bCompabilityMode = (qs.value("value").toString() == "true");
    }

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
                break;
            }

        }
        a.endArray();
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

    QMap<QString, QVariant> params;

    QVariant v1 = a.arguments().at(0);
    const QDBusArgument &a1 = v1.value<QDBusArgument>();

    if(a1.currentType() == QDBusArgument::ArrayType) {
        a1.beginArray();

        while(!a1.atEnd()) {
            qDebug() << "s2 " << a1.currentSignature();
            qDebug() << "s2 " << a1.currentType();
            const QDBusArgument &a2 = a1.asVariant().value<QDBusArgument>();

            if(a2.currentType() == QDBusArgument::StructureType) {

                a2.beginStructure();

                QDBusObjectPath objPath;
                a2 >> objPath;

                a2.beginMap();

                while(!a2.atEnd()) {

                    a2.beginMapEntry();

                    QString t;
                    QVariant v;
                    a2 >> t >> v;
                    qDebug() << t << " = " << v;
                    params.insert(t, v);
                    a2.endMapEntry();
                }

                a2.endMap();
                a2.endStructure();
            }
        }
        a1.endArray();

        QString callingType = params.value("org.freedesktop.Telepathy.Channel.ChannelType").value<QString>();
        QString callingNr = params.value("org.freedesktop.Telepathy.Channel.InitiatorID").value<QString>();

        if(callingNr == "<SelfHandle>") {
            qDebug() << "outgoing call ignored.";
            return ;
        }

        // check type of channel that has opened

        qDebug() << "check settings";

        QSqlQuery qs;
        qs.exec("SELECT key, value FROM setting");

        bool activateOnSms = false, activateOnlyUnknown = true, enableRoaming = false;

        while(qs.next()) {
            if(qs.value("key").toString() == "activate_only_unknown")
                activateOnlyUnknown = (qs.value("value").toString() == "true");

            if(qs.value("key").toString() == "activate_on_sms")
                activateOnSms = (qs.value("value").toString() == "true");

            if(qs.value("key").toString() == "enable_roaming")
                enableRoaming = (qs.value("value").toString() == "true");

            if(qs.value("key").toString() == "compability_mode")
                m_bCompabilityMode = (qs.value("value").toString() == "true");
        }

        qDebug() << "got settings " << activateOnlyUnknown << activateOnSms << m_bCompabilityMode;


        if(callingType == "org.freedesktop.Telepathy.Channel.Type.StreamedMedia") {
            qDebug() << "incoming call";
        }

        if(callingType == "org.freedesktop.Telepathy.Channel.Type.Text") {
            qDebug() << "incoming SMS (ignore!)";
            if(!activateOnSms)
                return ;
        }

        if(!enableRoaming && dbus::Instance()->isRoaming()) {
            qDebug() << "disabled when roaming!";
            return ;
        }

        if(activateOnlyUnknown) {
            QSqlDatabase db_contacts = QSqlDatabase::addDatabase("QSQLITE", "db_contacts");
            db_contacts.setDatabaseName("/home/nemo/.local/share/system/Contacts/qtcontacts-sqlite/contacts.db");
            db_contacts.open();


            QSqlQuery cq(db_contacts);

            cq.prepare("SELECT contactId FROM PhoneNumbers WHERE normalizedNumber = ?");
            cq.addBindValue(phonenumber::process(callingNr));
            if(!cq.exec()) {
                qDebug() << "error " << cq.lastError().text();
            }

            while(cq.next()) {
                qDebug() << "found existing contact with id " << cq.value("contactId").toInt();
                db_contacts.close();
                return ;
            }

            db_contacts.close();
        }


        qDebug() << "calling number = " << callingNr;

        phonenumber ph = phonenumber(callingNr,
                                     phonenumber::mobilecc_to_iso32662( dbus::Instance()->mobileCountryCode() ),
                                     QString::number( dbus::Instance()->mobileNetworkCode() ));

        qDebug() << "after localizing " << ph.number_local;

        //QDBusInterface showMsg("")

        qDebug() << "request raise/activate";

        dbus_adapter::Instance()->set_lookupState("activate:lookup");

        lookup_thread *lt = new lookup_thread();

        lt->start(ph, QList<int>());

    }




}
