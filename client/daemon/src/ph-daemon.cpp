#include <sailfishapp.h>
#include <QGuiApplication>
#include <QDebug>
#include <unistd.h>
#include <sys/types.h>
#include <grp.h>
#include <pwd.h>
#include <QDBusMessage>
#include <QList>
#include <QMetaType>
#include <QTextStream>
#include <QtDebug>

#include "dbus.h"
#include "db.h"
#include "phonenumber.h"
#include "overlay.h"
#include "auto_update.h"

void myMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context)
    QString txt;
    switch (type) {
    case QtInfoMsg:
        break;
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

   // setuid(0);
    qDebug() << "daemon starting?";

    QGuiApplication* app = SailfishApp::application(argc, argv);


    qDebug() << "open db";

    db::Instance();
    auto_update::Instance();


    QString dataDir = QDir::home().absolutePath() + "/.phonehook";
    QDir::home().mkpath(dataDir);

    qDebug() << "create listener";
    dbus listener(app);

    QDBusConnection::sessionBus().registerObject("/", app);
    QDBusConnection::sessionBus().registerService("com.omnight.phonehook");

    qRegisterMetaType<QMap<QString,QString> >("QMap<QString,QString>");


    // init overlay
    overlay o;
    //o.show();

    return app->exec();

}



