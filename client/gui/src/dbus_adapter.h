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
                "<signal name=\"lookupStateChanged\">\n"
                "<arg type=\"s\"/>\n"
                "</signal>\n"
                "<signal name=\"qmlResult\">\n"
                "<arg type=\"s\"/>\n"
                "</signal>\n"                
                "</interface>\n"
                "")

private:
    static dbus_adapter *m_Instance;
    explicit dbus_adapter(QObject *parent = 0);
    QList<QString> qmlItems;

    bool m_bIsReady;
    QString lookupState;

public:
    static dbus_adapter *Instance(QObject *parent = NULL);
    void clearReady();

    void set_lookupState(QString state);

    void sendQml(QString qml);

Q_SIGNALS:
    void lookupStateChanged(QString state);
    void qmlResult(QString state);

public Q_SLOTS:
    void Hello();
    void getLookupState();
    void ready();

};

#endif // DBUS_ADAPTER_H
