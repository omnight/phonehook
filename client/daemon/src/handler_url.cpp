#include "handler_url.h"
#include "robot_base.h"
#include <QNetworkProxy>
#include "compression.h"
#include <QSqlQuery>
#include <QNetworkCookie>

handler_url::handler_url(int botId, robot_base *parent)
    : QObject(parent),
      owner(parent),
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
    owner->expand(nextUrl);

    // find earlier URL to use as relative URL
    QUrl urlBase;
    process_data *pp = inputData;
    while(pp != NULL && pp->url.isEmpty())
        pp = (process_data*)pp->parent();

    if(pp) urlBase = pp->url;

    QString method = robotXml.attribute("postrequest", "false") == "true" ? "post" : "get";
    QString postdata = robotXml.attribute("postdata", "");
    owner->expand(postdata);


    QString hashKey = "";
    int cacheMax = 0;
    bool hasCache = false;

    QRegularExpression timeSpanRx(R"(^(?:(?<day>\d+)?(?:\.|$))?(?:(?<hour>\d\d?)(?::(?<minute>\d\d?)(?::(?<second>\d\d?)(?:\.(?<ms>\d+))?)?)?)?$)");

    QRegularExpressionMatch timeSpanMatch =
            timeSpanRx.match(robotXml.attribute("httpcache", "0"));

    qDebug() << "http cache = " << robotXml.attribute("httpcache", "0");

    if(timeSpanMatch.hasMatch()) {
        QString day_s = timeSpanMatch.captured("day");
        QString hour_s = timeSpanMatch.captured("hour");
        QString minute_s = timeSpanMatch.captured("minute");
        QString second_s = timeSpanMatch.captured("second");
        //QString ms_s = timeSpanMatch.captured("ms");

        if(!day_s.isNull()) cacheMax += day_s.toInt() * 24 * 3600;
        if(!hour_s.isNull()) cacheMax += hour_s.toInt() * 3600;
        if(!minute_s.isNull()) cacheMax += minute_s.toInt() * 60;
        if(!second_s.isNull()) cacheMax += second_s.toInt();

        if(cacheMax > 0) {
            qDebug() << "max cache (s)" << cacheMax;

            QByteArray hashData;
            QString rv = robotXml.attribute("requestvalue");
            owner->expand(rv);
            hashData.append("rv:" + rv);
            hashData.append("post:" + postdata);
            hashData.append("encoding:" + robotXml.attribute("encoding", ""));
            hashData.append("wheaders:" + robotXml.attribute("includeheaders", "false").toLower());

            hashKey = QCryptographicHash::hash(hashData,QCryptographicHash::Md5).toHex();

            qDebug() << "hashkey is" << hashKey;
        }
    }

    // try cache lookup if we have a hash key and max cache age
    if(hashKey != "" && cacheMax != 0) {

        QSqlQuery sq(R"(
            SELECT result,
                   CAST(strftime('%s', CURRENT_TIMESTAMP) AS INTEGER)
                   - CAST(strftime('%s',cache_date) AS INTEGER) cache_age
            FROM bot_response_cache
            WHERE bot_id=? AND httpRequestKey=?
            AND (CAST(strftime('%s', CURRENT_TIMESTAMP) AS INTEGER)
                 - CAST(strftime('%s',cache_date) AS INTEGER) < ?)
        )");

        sq.addBindValue(botId);
        sq.addBindValue(hashKey);
        sq.addBindValue(cacheMax);

        sq.exec();

        if(sq.next()) {
            qDebug() << "using data from bot_response_cache " << sq.value("cache_age").toInt() << " s old";
            pd->value = QString::fromUtf8( qUncompress(sq.value("result").toByteArray()) );
            hasCache = true;
        }
    }

    if(!hasCache) {
        while(nextUrl != "") {

            // support relative paths
            urlBase = urlBase.resolved(nextUrl);
            pd->url = urlBase;

            qDebug() << "LOADING" << urlBase.toString();

            QNetworkRequest req;
            req.setUrl(urlBase.toString());

            if(robotXml.attribute("requestfields", "") == "Header") {
                QString header = robotXml.attribute("requestvalue");
                owner->expand(header);

                foreach(QString line, header.split("\n", QString::SkipEmptyParts)) {
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

                nextUrl = "";

                qDebug() << "status code" << reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );

                if(robotXml.attribute("followredirect", "true").toLower() == "true") {
                    nextUrl = reply->rawHeader("Location");
                    postdata = "";
                    method = "get";
                }
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

        if(hashKey != "" && cacheMax > 0) {

            QSqlQuery sq(R"(
                DELETE FROM bot_response_cache WHERE bot_id=? AND httpRequestKey=?
            )");
            sq.addBindValue(botId);
            sq.addBindValue(hashKey);
            sq.exec();

            sq.prepare(R"(
                INSERT INTO bot_response_cache(bot_id,httpRequestKey,result)
                VALUES(?,?,?)
            )");
            sq.addBindValue(botId);
            sq.addBindValue(hashKey);
            sq.addBindValue(qCompress(pd->value.toUtf8()));
            sq.exec();
        }

    }

    qDebug() << pd->value.length() << "bytes" << pd->value.left(200);

    pd->node_id = robotXml.attribute("id", "");

    if(!hasCache)
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


QString handler_url::getCookie(QString key) {

    foreach (QNetworkCookie c,  cookieStore.allCookies()) {
        if(c.name() == key) return c.value();
    }

    return "";
}
