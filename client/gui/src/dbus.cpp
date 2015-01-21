#include "dbus.h"
#include <QDebug>
#include "lookup_thread.h"

dbus::dbus(QObject *parent) :
    QObject(parent)
{

    qDebug() << "registering telepathy";
    qDebug() << "connect status = " <<
                QDBusConnection::sessionBus().connect("",
                                                      "/org/freedesktop/Telepathy/Connection/ring/tel/ring", ///ring/tel/ring
                                    "org.freedesktop.Telepathy.Connection.Interface.Requests",
                                    "NewChannels",
                                    this, SLOT(onIncomingCall(const QDBusMessage&)));

    adapter = dbus_adapter::Instance(parent);

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

        QString callingNr = params.value("org.freedesktop.Telepathy.Channel.InitiatorID").value<QString>();

        qDebug() << "calling number = " << callingNr;

        //QDBusInterface showMsg("")

        qDebug() << "request raise/activate";

        //callingNr = "0752433760";

        dbus_adapter::Instance()->clearReady();
        dbus_adapter::Instance()->set_lookupState("event1");

        lookup_thread *lt = new lookup_thread(callingNr);

        lt->start();

    }




}
