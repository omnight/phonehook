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
#include "dbus_adapter.h"

class delayedInject: public QObject {
    Q_OBJECT

public:
    explicit delayedInject(QObject *parent = 0) {
        first = true;
    }

private:
    bool first;

    QQueue<QString> removeQueue;
    QThread removeThread; // can take a few secs, so do it in another thread


    public slots:

    void threadStart() {
        while(removeQueue.count() > 0) {
            QFile df(removeQueue.dequeue() );
            df.remove();
        }
    }

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
                removeQueue.enqueue(dataDir + "/" + fn);
            }

            connect(&removeThread, SIGNAL(started()), this, SLOT(threadStart()));
            removeThread.start();

            QFile::link(appDir + "/gui.qml", dataDir + "/" + rnf);

            qDebug() << "injecting";

            QDBusMessage qm = QDBusMessage::createMethodCall("com.omnight.lipstick", "/", "com.jolla.lipstick.ConnectionSelectorIf", "inject" );
            QVariantList injectArgs;
            injectArgs.append(dataDir + "/" + rnf);
            qm.setArguments(injectArgs);
            QDBusConnection::sessionBus().send(qm);

            // option 2 (non working) - interface signal
            //emit dbus_adapter::Instance()->inject(dataDir + "/" + rnf);

            qDebug() << "ready";
        } else {

            qDebug() << "ping!";

        }

        first = false;



    }

};

#endif // INJECT_H
