#ifndef DBUS_ADAPTER_H
#define DBUS_ADAPTER_H

#include <QDBusAbstractAdaptor>
#include <QTimer>
#include <QQueue>
#include <QDBusReply>

class dbus_adapter : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.omnight.phonehook")
    Q_CLASSINFO("D-Bus Introspection", ""
                "<interface name=\"com.omnight.phonehook\">\n"
                "<method name=\"Hello\">\n"
                "</method>\n"
                "<method name=\"getLookupState\">\n"
                "</method>\n"
                "<method name=\"ready\">\n"
                "</method>\n"
                "<method name=\"testLookup\">\n"
                "<arg direction=\"in\" name=\"number\" type=\"s\"/>\n"
                "<arg direction=\"in\" name=\"botid\" type=\"i\"/>\n"
                "</method>\n"
                "<method name=\"search\">\n"
                "<arg direction=\"in\" name=\"parameters\" type=\"a{sv}\"/>\n"
                "<arg direction=\"in\" name=\"bots\" type=\"av\"/>\n"
                "</method>"
                "<signal name=\"searchResult\">\n"
                "<arg name=\"xml\" type=\"s\" direction=\"out\">\n"
                "</signal>\n"
                "<signal name=\"searchState\">\n"
                "<arg name=\"state\" type=\"s\" direction=\"out\">\n"
                "</signal>\n"
                "<method name=\"notificationAction\">\n"
                "</method>\n"
                "</interface>\n"
                "")

//    "<signal name=\"inject\">\n"
//    "<arg name=\"file\" type=\"s\" direction=\"out\">\n"
//    "</signal>\n"
//    "<signal name=\"command\">\n"
//    "<arg name=\"fn\" type=\"s\" direction=\"out\">\n"
//    "<arg name=\"data\" type=\"s\" direction=\"out\">\n"
//    "</signal>\n"


private:
    static dbus_adapter *m_Instance;
    explicit dbus_adapter(QObject *parent = 0);
    QList<QString> qmlItems;

    QString lookupState;

    QStringList compabilityNotificationFields;
    QQueue< QPair<QString, QString> > compabilityNotificationQueue;
    QTimer compabilityNotificationTimer;

    uint m_compabilityNotificationId;

public:
    static dbus_adapter *Instance(QObject *parent = NULL);

    void set_lookupState(QString state);
    void sendQml(QString qml);
    void compabilityNotification(QString jsonData);
    Q_INVOKABLE void messageToPopup(QString function, QString data);

public Q_SLOTS:
    void Hello();
    void getLookupState();
    void testLookup(QString number, int botid);

    void search(QMap<QString,QVariant> parameters, QList<QVariant> bots);

    void compabilityNotificationTimerTick();

    void notificationCallback(uint);
    void notificationAction();
    void notificationClosed(uint id, uint uh);

signals:
    void searchResult(QString xml);
    void command(QString fn, QString data);
    void inject(QString qml);
    void searchState(QString state);

};

#endif // DBUS_ADAPTER_H
