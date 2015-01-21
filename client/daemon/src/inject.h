#ifndef INJECT_H
#define INJECT_H
#include <QObject>
#include <QDir>
#include <QDebug>
#include <QFile>
#include <QDBusMessage>
#include <QUuid>
#include <QDBusConnection>
#include <QThread>

class delayedInject: public QObject {
    Q_OBJECT

public:
    explicit delayedInject(QObject *parent = 0) {
        first = true;
    }

private:
    bool first;

    public slots:
    void ready() {

        if(first) {

            QString dataDir = QDir::home().absolutePath() + "/.phonehook";
            QString appDir = "/usr/share/phonehook/qml/popup";
            QString rnf = QUuid::createUuid().toString() + ".qml";

            qDebug() << rnf;

            QStringList filter;
            filter.append("{*}.qml");
            foreach(QString fn, QDir(dataDir).entryList( filter )) {
                qDebug() << "remove" << fn;
                QFile df(dataDir + "/" + fn);
                df.remove();
            }

            QFile::link(appDir + "/gui.qml", dataDir + "/" + rnf);

            QDBusMessage qm = QDBusMessage::createMethodCall("com.omnight.lipstick", "/", "com.jolla.lipstick.ConnectionSelectorIf", "inject" );
            QVariantList injectArgs;
            injectArgs.append(dataDir + "/" + rnf);
            qm.setArguments(injectArgs);
            QDBusConnection::sessionBus().call(qm);


            qDebug() << "ready";
        } else {

            qDebug() << "ping!";

        }

        first = false;



    }

};

#endif // INJECT_H
