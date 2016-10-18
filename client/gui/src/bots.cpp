#include <QDir>
#include <QDomDocument>
#include <QXmlQuery>
#include <QSqlField>
#include <QXmlResultItems>
#include <QDBusMessage>
#include <QDBusConnection>
#include <QProcess>
#include <QDBusConnectionInterface>
#include <QSqlError>

#include "bots.h"
#include "setting.h"
#include "countries.h"
#include "lookup_thread.h"

bots::bots(QObject *parent) :
    QObject(parent)
{

    QString dataDir = QDir::home().absolutePath() + "/.phonehook";
    QDir::home().mkpath(dataDir);
    m_version = 0;

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dataDir + "/phonehook.db");

    m_db.open();

    // enable foreign key support
    m_db.exec("PRAGMA foreign_keys = ON;");

    if(m_db.tables().count() > 0) {
        QSqlQuery getVersionQuery("SELECT * FROM DB_VERSION");
        if(getVersionQuery.next()) {
          m_version = getVersionQuery.value(0).toInt();
        }
    }

    // check to see if injector DBus Adaptor in active
    connect(&serviceWatcher, SIGNAL(serviceRegistered(QString)), this, SLOT(service_registered(QString)));
    connect(&serviceWatcher, SIGNAL(serviceUnregistered(QString)), this, SLOT(service_unregistered(QString)));

    connect(&botDownloader, SIGNAL(botDownloadFailure()), this, SLOT(botDownload_fail()));
    connect(&botDownloader, SIGNAL(botDownloadSuccess(int)), this, SLOT(botDownload_finish(int)));

//    serviceWatcher.setWatchMode(QDBusServiceWatcher::WatchForRegistration);
    serviceWatcher.setConnection(QDBusConnection::sessionBus());
    serviceWatcher.addWatchedService("com.omnight.phonehook");

    m_daemonActive = QDBusConnection::sessionBus().interface()->isServiceRegistered("com.omnight.phonehook");
    emit daemonActive_changed(m_daemonActive);


    m_testSources = (setting::get("source_test", "false") == "true");

    // auto-start daemon if not running
    if(!m_daemonActive) {
        startDaemon();
    }

}

bots::~bots() {
    m_db.close();
}

int bots::version() {
    return m_version;
}

void bots::downloadBot(QString file, bool KeepData) {
    botDownloader.download(file, KeepData);
}

void bots::botDownload_fail() {
    emit botDownloadFailure();
}

void bots::botDownload_finish(int botId) {
    botList()->refresh();
    emit botList_changed(&m_botList);
    emit botList()->count_changed(botList()->rowCount());
    emit botDownloadSuccess(botId);
}

QVariantMap bots::recordToVariantMap(QSqlRecord r) {
    QVariantMap vm;

    for(int i=0; i < r.count(); i++) {
        QSqlField f = r.field(i);
        vm.insert(f.name(), f.value());
    }

    return vm;

}


QVariantMap bots::getBotDetails(int botId) {

    QSqlQuery sq;

    sq.prepare("SELECT * FROM bot WHERE id = ?");
    sq.addBindValue(botId);
    sq.exec();

    if(sq.next()) {
        return recordToVariantMap(sq.record());
    }

}

void bots::setBotSearchListTag(QString tag) {
    m_botSearchList.setQuery("SELECT bot.id, bot.name FROM bot JOIN bot_tag ON bot.id = bot_tag.bot_id WHERE bot_tag.tag='" + tag + "';");
    emit botSearchList_changed(&m_botSearchList);
}

void bots::setActiveBot(int botId) {

    qDebug() << "setting active bot" << botId;

    m_paramList.setQuery("SELECT * FROM bot_param WHERE bot_id = " + QString::number(botId));
    m_loginList.setQuery("SELECT * FROM bot_login WHERE bot_id = " + QString::number(botId));

    emit paramList_changed(&m_paramList);
    emit loginList_changed(&m_loginList);

}

void bots::setBotParam(int botId, QString key, QString value) {
    QSqlQuery sq;

    sq.prepare("UPDATE bot_param SET value = ? WHERE bot_id = ? AND key = ?;");
    sq.addBindValue(value);
    sq.addBindValue(botId);
    sq.addBindValue(key);
    qDebug() << "update bot param " << sq.exec();
}

void bots::testBot(int botId, QString testNumber) {


    QVariantList args;
    QDBusMessage testcall;
    args.append(testNumber);

    if(botId == 0) {
        testcall =
        QDBusMessage::createMethodCall("com.omnight.phonehook",
                                       "/",
                                       "com.omnight.phonehook",
                                       "testLookup2");
    } else {
        testcall =
        QDBusMessage::createMethodCall("com.omnight.phonehook",
                                       "/",
                                       "com.omnight.phonehook",
                                       "testLookup");
        args.append(botId);
    }

    testcall.setArguments(args);
    QDBusConnection::sessionBus().send(testcall);

}


void bots::service_registered(QString serviceName) {
    qDebug() << "registered" << serviceName;

    if(serviceName == "com.omnight.phonehook") {
        m_daemonActive = true;
        emit daemonActive_changed(m_daemonActive);

        // db may have updated?

        QSqlQuery qs;
        qs.exec("SELECT version FROM db_version");
        while(qs.next()) {
            m_version = qs.value("version").toInt();
        }

        initBotList();
    }
}

void bots::initBotList() {
    if(!m_botList.query().isValid())
        m_botList.setQuery("SELECT id, name, revision FROM bot;");
    else
        m_botList.refresh();

    emit botList_changed(&m_botList);
}


void bots::service_unregistered(QString serviceName) {
    qDebug() << "unregistered" << serviceName;
    if(serviceName == "com.omnight.phonehook") {
        m_daemonActive = false;
        emit daemonActive_changed(m_daemonActive);
    }
}

void bots::startDaemon() {
    QProcess p;
    p.startDetached("systemctl", QStringList() << "--user" << "restart" << "phonehook-daemon.service");
}


int bots::botStatusCompare(QString name, int rev) {

    //qDebug() << "check" << name << rev;

    QSqlQuery qs;
    qs.prepare("SELECT revision FROM bot WHERE name = ?");
    qs.addBindValue(name);
    qs.exec();

    if(qs.next()) {
        if(rev == qs.value("revision").toInt()) {
            return 1;
        } else
            return 2;
    } else {
        return 0;
    }


}
int bots::getBotId(QString name) {
    QSqlQuery qs;

    qs.prepare("SELECT id FROM bot WHERE name = ?");
    qs.addBindValue(name);

    qs.exec();
    if(qs.next()) {
        return qs.value("id").toInt();
    }

    return -1;
}

bool bots::removeBot(int botId) {
    QSqlQuery qs;

    // delete actual bot

    qs.prepare("DELETE FROM bot WHERE id = ?");       
    qs.addBindValue(botId);
    qs.exec();

    qDebug() << "delete result = " << qs.numRowsAffected();

    bool status = qs.numRowsAffected() > 0;

    m_botList.refresh();
    emit botList_changed(&m_botList);
    emit botList()->count_changed(botList()->rowCount());

    return status;
}

QString bots::getCountryName(QString code) {
    return countries::getCountryNameISO3166(code);
}

void bots::clearCache(int botId) {
    QSqlQuery qs;

    qs.exec("DELETE FROM bot_cookie_cache WHERE bot_id = " + QString::number(botId));
    qs.exec("DELETE FROM bot_response_cache WHERE bot_id = " + QString::number(botId));
    qs.exec("DELETE FROM bot_result_cache WHERE bot_id = " + QString::number(botId));

}

bool bots::hasBlockTag(int botId) {
    QSqlQuery qs("SELECT * FROM bot_tag WHERE bot_id = ? AND tag='block';");
    qs.addBindValue(botId);
    qs.exec();

    return qs.next();
}

bool bots::isBlockSource(int botId) {
    QSqlQuery qs("SELECT * FROM block WHERE bot_id = ? AND type=2;");
    qs.addBindValue(botId);
    qs.exec();

    return qs.next();
}

void bots::setBlockSource(int botId, bool enabled) {

    if(!enabled) {
        QSqlQuery qs("DELETE FROM block WHERE bot_id = ? AND type=2;");
        qs.addBindValue(botId);
        qs.exec();
    } else {
        QSqlQuery qs("INSERT INTO block(type,bot_id) VALUES(2, ?);");
        qs.addBindValue(botId);
        qs.exec();
    }

}

void bots::copyCookies(int bot_id) {

    // open cookiez db
    // /home/nemo/.local/share/phonehook/phonehook/.QtWebKit/cookies.db


    // table|cookies|cookies|2|CREATE TABLE cookies (cookieId VARCHAR PRIMARY KEY, cookie BLOB)
    // index|sqlite_autoindex_cookies_1|cookies|3|

    QString cookies_db_path = QDir::home().absolutePath() + "/.local/share/phonehook/phonehook/.QtWebKit/cookies.db";

    QSqlDatabase cookieDb = QSqlDatabase::addDatabase("QSQLITE", "cookies");

    cookieDb.setDatabaseName(cookies_db_path);
    cookieDb.open();

    QSqlQuery getCookiesQuery("SELECT cookie FROM cookies;", cookieDb);

    getCookiesQuery.exec();



    while(getCookiesQuery.next()) {
        // get cookie text from BLOB
        QString cookieText = QString::fromUtf8( getCookiesQuery.value("cookie").value<QByteArray>() );

        // split
        // SSID=A...9; secure; HttpOnly; expires=Wed, 05-Jul-2017 16:56:19 GMT; domain=.google.com; path=/
        QStringList parts = cookieText.split(";");
/*
        bot_id INTEGER REFERENCES BOT(id) ON DELETE CASCADE,
        key TEXT,
        domain TEXT,
        path TEXT,
        expire TEXT,
        value TEXT*/

        QString key = parts[0].split("=")[0];
        QString value = parts[0].split("=")[1];
        QString domain;
        QString path;
        QString expires;

        for(int i=1; i < parts.length(); i++) {
            QStringList kvp = parts[i].split("=");

            if(kvp[0].toLower().trimmed() == "domain") domain = kvp[1];
            if(kvp[0].toLower().trimmed() == "path") path = kvp[1];
            if(kvp[0].toLower().trimmed() == "expires") expires = kvp[1];
        }

        QSqlQuery clearOldCookie(R"(
            DELETE FROM bot_cookie_cache WHERE bot_id=? AND key=? AND domain=?;
        )");

        clearOldCookie.addBindValue(bot_id);
        clearOldCookie.addBindValue(key);
        clearOldCookie.addBindValue(domain);

        clearOldCookie.exec();

        QSqlQuery checkExistingCookie(R"(
            INSERT INTO bot_cookie_cache(bot_id,key,value,domain,path,expire)
            VALUES(?,?,?,?,?,?);
        )");


        if(expires == "") {
            expires = "session";
        } else {
            QDateTime expireDate = QDateTime::fromString(expires, "ddd, dd-MMM-yyyy HH:mm:ss 'GMT'");
            expireDate.setTimeSpec(Qt::UTC);
            expires = expireDate.toString("yyyy-MM-dd HH:mm:ss");
        }
        checkExistingCookie.addBindValue(bot_id);
        checkExistingCookie.addBindValue(key);
        checkExistingCookie.addBindValue(value);
        checkExistingCookie.addBindValue(domain);
        checkExistingCookie.addBindValue(path);
        checkExistingCookie.addBindValue(expires);
        checkExistingCookie.exec();

        qDebug() << "Cookie: " + key+"="+QUrl::toPercentEncoding(value)+"; Path=" + path + "; Domain=" + domain + "; Expires=" + expires+";";
        //qDebug() << bot_id << key << value << domain << path << expires;

    }

    cookieDb.close();
    cookieDb.removeDatabase("cookies");


}

void bots::vCardWrite(QString name, QStringList numbers, QString address) {
    QFile vCardFile("/home/nemo/.phonehook/phonehook.vcf");
    if(vCardFile.open(QIODevice::WriteOnly)) {
        QTextStream stream(&vCardFile);
        stream.setCodec("UTF-8");
        stream << "BEGIN:VCARD" << endl <<
                  "VERSION:3.0" << endl;

        if(!name.isEmpty()) stream << "FN:" << name << endl;
        if(!address.isEmpty()) stream << "ADR:" << address.replace("\r\n", " ").replace("\n", " ").replace("\r", " ") << endl;

        foreach(auto nr, numbers) {
            stream << "TEL:" << nr << endl;
        }

        stream << "END:VCARD";
        stream.flush();
        vCardFile.close();
    }
}
