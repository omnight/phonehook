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

#include <QTextStream>
#include <QtDebug>

void myMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString txt;
    switch (type) {
    case QtDebugMsg:
        txt = QString("Debug: %1").arg(msg);
        break;
    case QtWarningMsg:
        txt = QString("Warning: %1").arg(msg);
    break;
    case QtCriticalMsg:
        txt = QString("Critical: %1").arg(msg);
    break;
    case QtFatalMsg:
        txt = QString("Fatal: %1").arg(msg);
        abort();
    }
    QFile outFile("/home/nemo/ph-log.txt");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << endl;
}

int main(int argc, char *argv[])
{

    // enable for file logging
    //qInstallMessageHandler(myMessageHandler);

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



