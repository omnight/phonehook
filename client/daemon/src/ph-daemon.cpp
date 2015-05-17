#include <sailfishapp.h>
#include <QGuiApplication>
#include <QDebug>

#include <sys/types.h>
#include <grp.h>
#include <pwd.h>
#include "dbus.h"
#include "db.h"
#include <QDBusMessage>
#include <QList>

#include "phonenumber.h"
#include <QMetaType>

#include "overlay.h"

int main(int argc, char *argv[])
{

    setuid(0);
    qDebug() << "daemon starting?";
    QGuiApplication app(argc, argv);
    qDebug() << "open db";

    db::Instance(&app);

    QString dataDir = QDir::home().absolutePath() + "/.phonehook";
    QDir::home().mkpath(dataDir);

    qDebug() << "create listener";
    dbus listener(&app);

    QDBusConnection::sessionBus().registerObject("/", &app);
    QDBusConnection::sessionBus().registerService("com.omnight.phonehook");

    qRegisterMetaType<QMap<QString,QString> >("QMap<QString,QString>");
    rule::initialize();


    // init overlay
    overlay o;
    o.show();

    return app.exec();

}

