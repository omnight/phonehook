#ifndef HANDLER_URL_H
#define HANDLER_URL_H

#include <QObject>
#include "process_data.h"
#include <QtXml>
#include <QList>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <QNetworkCookieJar>

#include <QThread>

class handler_url;

class handler_url_thread: public QObject {
    Q_OBJECT

public:
    handler_url_thread(handler_url *parent): netman(this) {
        this->parent = parent;
    }

private:
    handler_url *parent;
    QNetworkAccessManager netman;

public slots:
    void beginRequest(const QNetworkRequest request, QObject* jar, QString method, QByteArray data);
    void network_response(QNetworkReply *reply);
    void run();

};

class handler_url : public QObject
{
    Q_OBJECT


private:

    QThread waitThread;
    handler_url_thread waitThreadObj;

    QMutex waitForReplyMutex;

public:
    QWaitCondition waitForReply;
    explicit handler_url(QObject *parent = 0);
    void loadUrl(const QDomElement &robotXml, process_data *inputData, process_data *pd);

    QNetworkReply *reply;

signals:

public slots:



};




#endif // HANDLER_URL_H
