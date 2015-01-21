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

private:
    QDBusInterface *interface;

    dbus_adapter *adapter;

signals:

    void activate(QString nr);

public slots:

    void onIncomingCall(const QDBusMessage &a);

};

#endif // DBUS_H
