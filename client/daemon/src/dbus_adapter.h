#ifndef DBUS_ADAPTER_H
#define DBUS_ADAPTER_H

#include <QDBusAbstractAdaptor>

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
                "</interface>\n"
                "")

private:
    static dbus_adapter *m_Instance;
    explicit dbus_adapter(QObject *parent = 0);
    QList<QString> qmlItems;

    QString lookupState;

public:
    static dbus_adapter *Instance(QObject *parent = NULL);

    void set_lookupState(QString state);
    void sendQml(QString qml);
    Q_INVOKABLE void messageToPopup(QString function, QString data);

public Q_SLOTS:
    void Hello();
    void getLookupState();
    void testLookup(QString number, int botid);

};

#endif // DBUS_ADAPTER_H
