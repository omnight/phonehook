#include "handler_url.h"
#include "robot_base.h"
#include <QNetworkProxy>

handler_url::handler_url(QObject *parent)
    : QObject(parent),
      waitThreadObj(this) {

    waitThreadObj.moveToThread(&waitThread);
    connect(&waitThread,SIGNAL(started()), &waitThreadObj, SLOT(run()));
    waitThread.start();

}


void handler_url_thread::run() {

    qDebug() << "running url event handler";

    connect(&netman, SIGNAL(finished(QNetworkReply*)), this, SLOT(network_response(QNetworkReply*)));

    QEventLoop loop;
    loop.exec();

}

void handler_url_thread::beginRequest(const QNetworkRequest request, QObject* jar, QString method, QByteArray data) {
    qDebug() << "begin request";
    netman.setCookieJar(static_cast<QNetworkCookieJar*>(jar));

    // set proxy for debugging (fiddler)
    //netman.setProxy(QNetworkProxy(QNetworkProxy::HttpProxy, "192.168.1.68", 8888));

    if(method == "get") {
        netman.get(request);
    }

    if(method == "post") {
        netman.post(request, data);
    }
}

void handler_url::loadUrl(const QDomElement &robotXml, process_data *inputData, process_data *pd) {

    QString nextUrl = pd->value;
    robot_base::expand(nextUrl);

    QList<process_data*> parents;
    process_data *par = static_cast<process_data*>(inputData);

    // inherit cookies
    while(par) {
        qDebug() << par;
        parents.prepend(par);
        par = static_cast<process_data*>(par->parent());
    }

    QNetworkCookieJar *jar = new QNetworkCookieJar();
    QUrl urlBase;

    foreach(process_data *p, parents) {
        if(!p->url.isEmpty()) urlBase = p->url;
        foreach(QNetworkCookie c, p->cookies) {
            qDebug() << "setting cookie " <<  c.name() << ":" << c.value() << c.path() << c.domain();
            jar->insertCookie(c);
        }
    }

    QString method = robotXml.attribute("postrequest", "false") == "true" ? "post" : "get";
    QString postdata = robotXml.attribute("postdata", "");
    robot_base::expand(postdata);

    while(nextUrl != "") {

        // support relative paths
        urlBase = urlBase.resolved(nextUrl);

        qDebug() << "LOADING" << urlBase.toString();

        pd->url = urlBase;

        QNetworkRequest req;
        req.setUrl(urlBase.toString());

        if(robotXml.attribute("requestfields", "") == "Header") {

            foreach(QString line, robotXml.attribute("requestvalue").split("\n", QString::SkipEmptyParts)) {
                qDebug() << "HEADER" << line;
                QStringList keyValue = line.split(":");
                if(keyValue.length() >= 2) {
                    QByteArray key = keyValue[0].toLocal8Bit();
                    keyValue.removeFirst();
                    QByteArray value = keyValue.join(":").toLocal8Bit();
                    req.setRawHeader(key, value);
                }
            }
        }

        waitForReplyMutex.lock();
        QMetaObject::invokeMethod(&waitThreadObj, "beginRequest",
                                  Q_ARG(const QNetworkRequest, req),
                                  Q_ARG(QObject*, jar),
                                  Q_ARG(QString, method),
                                  Q_ARG(QByteArray, postdata.toUtf8()));

        waitForReply.wait(&waitForReplyMutex);
        qDebug() << "wait finished";

        waitForReplyMutex.unlock();


        pd->cookies = reply->header(QNetworkRequest::SetCookieHeader).value< QList<QNetworkCookie> >();
        for(int i=0; i < pd->cookies.length(); i++) {
            qDebug() << "set-cookie" << pd->cookies.at(i).name();

            QNetworkCookie cookie = pd->cookies.at(i);
            if(cookie.domain() == "") cookie.setDomain(QUrl(nextUrl).host());
            pd->cookies.replace(i, cookie);

        }

        jar->deleteLater();
        this->reply->deleteLater();
        nextUrl = "";

        if(robotXml.attribute("followredirect", "true").toLower() == "true") {
            nextUrl = reply->header(QNetworkRequest::LocationHeader).value<QString>();
            postdata = "";
            method = "get";
        }

    }

    pd->node_id = robotXml.attribute("id", "");
    pd->value = reply->readAll();

}


void handler_url_thread::network_response(QNetworkReply *reply) {

    qDebug() << "got network response!";
    netman.cookieJar()->setParent(0);
    reply->setParent(NULL);
    reply->moveToThread(QCoreApplication::instance()->thread());
    parent->reply = reply;
    parent->waitForReply.wakeOne();
}
