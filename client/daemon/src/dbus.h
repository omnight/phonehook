#ifndef DBUS_H
#define DBUS_H
#include <QtDBus>
#include <QObject>

#include "dbus_adapter.h"

class dbus : public QObject
{
    Q_OBJECT

public:
    explicit dbus(QObject *parent = 0);

    static dbus *Instance() { return m_instance; }

    int mobileCountryCode() { return m_mobileCountryCode; }
    bool isRoaming() { return m_isRoaming; }

private:
    QDBusInterface *interface;
    dbus_adapter *adapter;
    static dbus *m_instance;
    void updateNetwork();

    QDateTime m_lastNetworkUpdate;

    QDBusObjectPath m_modemPath;
    bool m_isRoaming;
    int m_mobileCountryCode;
    int m_mobileNetworkCode;

signals:

    void activate(QString nr);

public slots:

    void onIncomingCall(const QDBusMessage &a);

    void gotModems(QDBusMessage reply);
    void gotNetworkStatus(QDBusMessage reply);

};

#endif // DBUS_H
