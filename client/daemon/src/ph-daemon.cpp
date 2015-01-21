#include <sailfishapp.h>
#include <QCoreApplication>
#include <QDebug>

#include <sys/types.h>
#include <grp.h>
#include <pwd.h>
#include "dbus.h"
#include "db.h"
#include <QDBusMessage>
#include <QList>
#include "inject.h"


int main(int argc, char *argv[])
{

    setuid(0);

    qDebug() << "daemon starting?";

    QCoreApplication app(argc, argv);

    qDebug() << "open db";

    db database(&app);

    qDebug() << "create listener";


    dbus listener(&app);

    QDBusConnection::sessionBus().registerObject("/", &app);
    QDBusConnection::sessionBus().registerService("com.omnight.phonehook");

    QString dataDir = QDir::home().absolutePath() + "/.phonehook";
    QDir::home().mkpath(dataDir);

    delayedInject d;
//    QTimer t;

//    t.setInterval(1000);
//    t.setSingleShot(true);
//    d.connect(&t, SIGNAL(timeout()), &d, SLOT(ready()));

//    t.start();

    d.ready();

    return app.exec();

}

