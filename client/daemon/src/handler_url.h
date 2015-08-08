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
class robot_base;

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

class response_wrapper: public QObject {
    Q_OBJECT

public:
    response_wrapper(QObject *parent): QObject(parent) { }

    // required: Headers (cookies), response data

    enum field_type {
        Unknown,
        Header,
        Cookie,
        Data
    };

    void saveToStream(QDataStream &stream);
    void loadFromStream(QDataStream &stream);

    void loadFromReply(QNetworkReply *reply);

    QList<QNetworkCookie> cookies;
    QList< QPair<QByteArray,QByteArray> > headers;

};


class CookieMonster: public QNetworkCookieJar {
    Q_OBJECT

private:
    int botId;

public:

    virtual QList<QNetworkCookie> cookiesForUrl(const QUrl &url) const;

    QList<QNetworkCookie> allCookies() { return QNetworkCookieJar::allCookies(); }
    explicit CookieMonster(int botId, QObject *parent = 0);
    ~CookieMonster();
};

class handler_url : public QObject
{
    Q_OBJECT


private:

    QThread waitThread;
    handler_url_thread waitThreadObj;
    QMutex waitForReplyMutex;
    CookieMonster cookieStore;
    robot_base *owner;
    int botId;

public:
    QWaitCondition waitForReply;
    explicit handler_url(int botId, robot_base *parent = 0);
    void loadUrl(const QDomElement &robotXml, process_data *inputData, process_data *pd);
    QString getCookie(QString key);

    QNetworkReply *reply;

    ~handler_url();

signals:

};




#endif // HANDLER_URL_H
