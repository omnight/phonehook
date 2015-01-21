#include "handler_url.h"
#include "robot_base.h"
#include <QNetworkProxy>
#include "compression.h"
#include <QSqlQuery>

handler_url::handler_url(int botId, QObject *parent)
    : QObject(parent),
      waitThreadObj(this),
      cookieStore(botId) {

    this->botId = botId;

    waitThreadObj.moveToThread(&waitThread);
    connect(&waitThread,SIGNAL(started()), &waitThreadObj, SLOT(run()));
    waitThread.start();

}


void handler_url_thread::run() {

    qDebug() << "running url event handler, thread id = " << QThread::currentThreadId();

    netman.moveToThread(QThread::currentThread());

    connect(&netman, SIGNAL(finished(QNetworkReply*)), this, SLOT(network_response(QNetworkReply*)));
    QEventLoop loop;

    loop.exec();

    qDebug() << "loop is loose!";

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

    QUrl urlBase;
    QFile cacheFile;

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

        // calculate request hash


        int cacheMaxAge = robotXml.attribute("cache", "0").toInt();
        QByteArray cacheData;
        bool hasCache = false;

//        if(cacheMaxAge != 0) {

            QByteArray hashData;

            foreach(QByteArray b, req.rawHeaderList()) {
                hashData.append(b);
            }

            foreach(QNetworkCookie c, cookieStore.cookiesForUrl(urlBase)) {
                hashData.append(c.name());
                hashData.append(":");
                hashData.append(c.value());
            }

            hashData.append(postdata);

            QString hashKey = QCryptographicHash::hash(hashData,QCryptographicHash::Md5).toHex();

            qDebug() << "hashkey is" << hashKey;

            cacheFile.setFileName( "/home/nemo/.phonehook/cache/" + hashKey );

            if(cacheFile.exists()) {
//                QFileInfo fi(cacheFile);
//                long age =
//                fi.lastModified().secsTo(QDateTime::currentDateTime());

//                if(age / 60 < cacheMaxAge) {
//                    cacheData = cacheFile.readAll();
//                    hasCache = true;
//                } else {
                    cacheFile.remove();
//                }
            }
//        }

        foreach(QNetworkCookie c, cookieStore.cookiesForUrl(urlBase)) {
            qDebug() << "COOKIE" << c.name() << ":" << c.value();
        }

        if(!hasCache) {

            waitForReplyMutex.lock();
            QMetaObject::invokeMethod(&waitThreadObj, "beginRequest",
                                      Q_ARG(const QNetworkRequest, req),
                                      Q_ARG(QObject*, &cookieStore),
                                      Q_ARG(QString, method),
                                      Q_ARG(QByteArray, postdata.toUtf8()));

            waitForReply.wait(&waitForReplyMutex);
            qDebug() << "wait finished";

            waitForReplyMutex.unlock();

        }

        //pd->cookies.append( reply->header(QNetworkRequest::SetCookieHeader).value< QList<QNetworkCookie> >() );

//        for(int i=0; i < pd->cookies.length(); i++) {
//            QNetworkCookie cookie = pd->cookies.at(i);
//            if(cookie.domain() == "") cookie.setDomain(QUrl(urlBase).host());
//            pd->cookies.replace(i, cookie);

//            qDebug() << "set-cookie" << pd->cookies.at(i).name() << pd->cookies.at(i).value() << pd->cookies.at(i).domain();
//            jar->insertCookie(cookie);
//        }

        nextUrl = "";

        qDebug() << "status code" << reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );

        if(robotXml.attribute("followredirect", "true").toLower() == "true") {
            nextUrl = reply->rawHeader("Location");
            postdata = "";
            method = "get";       
        }

        if(nextUrl != "") reply->deleteLater();

    }

    QByteArray responseBytes;
    QByteArray rawResponse = reply->readAll();
    QString encoding = reply->rawHeader("Content-Encoding").toLower();

    qDebug() << "content-encoding" << encoding << "encoding" << robotXml.attribute("encoding", "");

    if(encoding == "gzip") {
        responseBytes = Compression::gUncompress(rawResponse);
        if(responseBytes.length() == 0) {
            responseBytes = rawResponse;
        } else {
            qDebug() << "warning: QNetworkReply gzip uncompression failed. manual un-gzip.";
        }
    } else  {
        if(encoding != "")
            qDebug() << "warning: unsupported encoding " << encoding;
        responseBytes = rawResponse;
    }

    if(robotXml.attribute("encoding", "") != "") {
        QTextDecoder *dec = QTextCodec::codecForName(robotXml.attribute("encoding", "").toLocal8Bit())->makeDecoder();
        pd->value = dec->toUnicode(responseBytes);
        delete dec;
    } else {
        pd->value = responseBytes;
    }


    if( robotXml.attribute("includeheaders", "false").toLower() == "true" ) {
        QString headers;
        for(int i=0; i < reply->rawHeaderPairs().length(); i++) {
            QPair< QByteArray, QByteArray > p = reply->rawHeaderPairs()[i];
            headers += p.first + ": " + p.second + "\n";
        }

        headers += "\n";
        pd->value = headers + pd->value;
    }


    // save cache
//    cacheFile.open(QFile::WriteOnly);
//    cacheFile.write(pd->value.toUtf8());
//    cacheFile.flush();
//    cacheFile.close();


    qDebug() << pd->value.length() << "bytes" << pd->value.left(200);

    pd->node_id = robotXml.attribute("id", "");

    reply->deleteLater();

}


void handler_url_thread::network_response(QNetworkReply *reply) {

    qDebug() << "got network response!";
    netman.cookieJar()->setParent(0);
    reply->setParent(NULL);
    reply->moveToThread(QCoreApplication::instance()->thread());
    parent->reply = reply;
    parent->waitForReply.wakeOne();

}

handler_url::~handler_url() {
    waitThread.exit();
    waitThread.wait();
}

void response_wrapper::saveToStream(QDataStream &stream) {

}

void response_wrapper::loadFromStream(QDataStream &stream) {

}

void response_wrapper::loadFromReply(QNetworkReply *reply) {

    cookies = reply->header(QNetworkRequest::SetCookieHeader).value< QList<QNetworkCookie> >();
    headers = reply->rawHeaderPairs();
}


CookieMonster::CookieMonster(int botId, QObject *parent): QNetworkCookieJar(parent) {
    // load cookies (?)
    this->botId = botId;
    qDebug() << "attempting load cookies " << botId;

    QSqlQuery getQuery("SELECT * FROM BOT_COOKIE_CACHE WHERE bot_id = ?;");

    getQuery.addBindValue(botId);
    getQuery.exec();

    while(getQuery.next()) {

        QNetworkCookie c;
        c.setName( getQuery.value("key").toByteArray() );
        c.setDomain( getQuery.value("domain").toString() );
        c.setPath( getQuery.value("path").toString() );

        if(getQuery.value("expire") != "session") {
            QDateTime date = QDateTime::fromString( getQuery.value("expire").toString(), "yyyy-MM-dd HH:mm:ss");
            date.setTimeSpec(Qt::UTC);
            c.setExpirationDate(date);
        }

        c.setValue( getQuery.value("value").toByteArray() );

        this->insertCookie(c);
    }

    qDebug() << "loaded" << allCookies().count() << "cookies";

}


CookieMonster::~CookieMonster() {
    // save

    qDebug() << "attempting save cookies";

    // BOT_COOKIE_CACHE(bot_id INTEGER, key TEXT, domain TEXT, path TEXT, expire TEXT, value TEXT)

    QSqlQuery clearQuery("DELETE FROM BOT_COOKIE_CACHE WHERE bot_id = ?;");
    clearQuery.addBindValue(botId);
    clearQuery.exec();

    qDebug() << "removed" << clearQuery.numRowsAffected() << "old cookies";

    foreach(QNetworkCookie cookie, allCookies()) {
        QSqlQuery insertQuery("INSERT INTO BOT_COOKIE_CACHE(bot_id, key, domain, path, expire, value) VALUES(?, ?, ?, ?, ?, ?);");

        insertQuery.addBindValue(botId);
        insertQuery.addBindValue(cookie.name());
        insertQuery.addBindValue(cookie.domain());
        insertQuery.addBindValue(cookie.path());
        insertQuery.addBindValue(cookie.isSessionCookie() ? "session" : cookie.expirationDate().toUTC().toString("yyyy-MM-dd HH:mm:ss") );
        insertQuery.addBindValue(cookie.value());

        qDebug() << "saved cookie!" <<
        insertQuery.exec();
    }

}

QList<QNetworkCookie> CookieMonster::cookiesForUrl(const QUrl &url) const {

//    qDebug() << "cookies for url" << url;

    QList<QNetworkCookie> result;

    foreach(QNetworkCookie c, QNetworkCookieJar::allCookies()) {

//        qDebug() << url.host().endsWith(c.domain()) <<
//                    url.path().startsWith( c.path() ) <<
//                    url.path() << c.path() <<
//                    c.isSessionCookie() <<
//                    c.isSecure() <<
//                    c.isHttpOnly() <<
//                    QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") <<
//                    c.expirationDate().toString("yyyy-MM-dd HH:mm:ss") <<
//                    (QDateTime::currentDateTime() < c.expirationDate());

        QUrl u = url;
        if(u.path() == "") u.setPath("/");

        if(u.host().endsWith(c.domain()) &&
           u.path().startsWith( c.path() ) &&
           (c.isSessionCookie() || QDateTime::currentDateTime() < c.expirationDate())) {

            result.append(c);

        }

    }

    return result;
}
