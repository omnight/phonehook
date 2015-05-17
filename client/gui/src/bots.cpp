#include "bots.h"
#include <QDir>
#include <QDomDocument>
#include <QXmlQuery>
#include <QSqlField>
#include <QXmlResultItems>
#include "lookup_thread.h"
#include <QDBusMessage>
#include <QDBusConnection>
#include <QProcess>
#include <QDBusConnectionInterface>
#include "countries.h"

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

    connect(&netman, SIGNAL(finished(QNetworkReply*)), this, SLOT(network_response(QNetworkReply*)));

    // check to see if injector DBus Adaptor in active
    connect(&serviceWatcher, SIGNAL(serviceRegistered(QString)), this, SLOT(service_registered(QString)));
    connect(&serviceWatcher, SIGNAL(serviceUnregistered(QString)), this, SLOT(service_unregistered(QString)));

//    serviceWatcher.setWatchMode(QDBusServiceWatcher::WatchForRegistration);
    serviceWatcher.setConnection(QDBusConnection::sessionBus());
    serviceWatcher.addWatchedService("com.omnight.phonehook");

    m_daemonActive = QDBusConnection::sessionBus().interface()->isServiceRegistered("com.omnight.phonehook");
    emit daemonActive_changed(m_daemonActive);


    m_testSources = (querySetting("source_test", "false") == "true");

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

void bots::downloadBot(QString file) {
    QNetworkRequest req;
    req.setUrl(QUrl("http://phonehook.omnight.com/get_bot.ashx?file=" + file + (testSources() ? "&beta=True" : "")));
    QNetworkReply* rep = netman.get(req);
}

QString bots::querySetting(QString key, QString def) {

    qDebug() << "getting" << key;

    QSqlQuery sq;
    sq.prepare("SELECT value FROM setting WHERE key = ?");
    sq.addBindValue(key);

    sq.exec();
    if(sq.next()) {
        return sq.value("value").toString();
    }

    return def;
}

void bots::setSetting(QString key, QString value) {
    qDebug() << "setting " << key << value;

    QSqlQuery sq;
    sq.prepare("INSERT OR REPLACE INTO setting (key, value) "
               "VALUES (?,?)");

    sq.addBindValue(key);
    sq.addBindValue(value);
    sq.exec();

    if(key == "source_test") {
        m_testSources = (value == "true");
        emit testSources_changed(m_testSources);
    }

}


void bots::updateBotData(QVariantMap data) {

    QString updateStr = "UPDATE bot SET ";

    QSqlQuery sq;

    for(QVariantMap::const_iterator iter = data.begin(); iter != data.end(); ++iter) {
        if(iter.key() == "id") continue ;
        updateStr = updateStr + iter.key() + " = ?,";
        //sq.addBindValue( iter.value() );
    }

    // remove last ,
    updateStr = updateStr.left( updateStr.length() - 1 );
    updateStr = updateStr + " WHERE id = ?";

    sq.prepare(updateStr);

    for(QVariantMap::const_iterator iter = data.begin(); iter != data.end(); ++iter) {
        if(iter.key() == "id") continue ;
        sq.addBindValue( iter.value() );
        qDebug() << iter.value();
    }

    sq.addBindValue(data.value("id"));

    qDebug() << data.value("id");


    qDebug() << "update query " << updateStr << sq.exec();

}

void bots::network_response(QNetworkReply *reply) {

    QXmlQuery xq;
    QSqlQuery sq;


    if(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute) != 200) {
        emit botDownloadFailure();
        return;
    }

    QString strXml = reply->readAll();

    xq.setFocus(strXml);

    QString name, revision, description, icon, link, country;
    xq.setQuery("robot/meta/name/string()");
    xq.evaluateTo(&name);
    name = name.trimmed();

    xq.setQuery("robot/meta/revision/number()");
    xq.evaluateTo(&revision);

    xq.setQuery("robot/meta/description/string()");
    xq.evaluateTo(&description);
    description = description.trimmed();

    xq.setQuery("robot/meta/country/string()");
    xq.evaluateTo(&country);
    country = country.trimmed();

    xq.setQuery("robot/meta/icon/string()");
    xq.evaluateTo(&icon);
    icon = icon.trimmed();

    xq.setQuery("robot/meta/link/string()");
    xq.evaluateTo(&link);
    link = link.trimmed();

    QXmlResultItems paramList;
    xq.setQuery("robot/meta/param");
    xq.evaluateTo(&paramList);

    QList<QVariantMap> insertBotParams;

    QXmlItem x = paramList.next();

    while(!x.isNull()) {
        if(x.isNode()) {
            QVariantMap param;

            QString key;
            QString title;
            QString type;

            xq.setFocus(x);
            xq.setQuery("./string()");
            xq.evaluateTo(&key);

            xq.setQuery("./@title/data(.)");
            xq.evaluateTo(&title);
            xq.setQuery("./@type/data(.)");
            xq.evaluateTo(&type);


            key = key.trimmed();
            title = title.trimmed();
            type = type.trimmed();

            if(type == "") type = "string";

            param.insert("key", key);
            param.insert("title", title);
            param.insert("type", type);

            insertBotParams.append(param);
        }
        x = paramList.next();
    }


    // evaluate and update tags

    QXmlResultItems sets;
    xq.setFocus(strXml);
    xq.setQuery("robot/set");
    xq.evaluateTo(&sets);

    QStringList tagList;

    while(!sets.next().isNull()) {
        xq.setFocus( sets.current() );
        QString tag;
        xq.setQuery("./@tags/data(.)");
        xq.evaluateTo(&tag);
        tagList.append( tag.split(",") );
    }

    sq.prepare("SELECT id FROM bot WHERE name = ?");
    sq.addBindValue( name );
    sq.exec();

    int existing_id = 0;

    while(sq.next()) {
        existing_id = sq.value("id").toInt();
    }

    sq.finish();

    if(existing_id) {

        sq.prepare("SELECT key, value FROM bot_param WHERE bot_id = ?");
        sq.addBindValue(existing_id);

        sq.exec();

        while(sq.next()) {

            QString key = sq.value("key").toString();
            QString value = sq.value("value").toString();

            for(int i=0; i < insertBotParams.length(); i++) {
                QVariantMap ip = insertBotParams.at(i);
                if(ip.value("key") == key) {
                    ip.insert("value", value);
                    insertBotParams.replace(i, ip);
                }
            }
        }

        sq.finish();

        sq.prepare("DELETE FROM bot_param WHERE bot_id = ?");
        sq.addBindValue(existing_id);

        qDebug() << "deleting old params" <<
                    sq.exec();


        sq.finish();

        sq.prepare("DELETE FROM bot_tag WHERE bot_id = ?");
        sq.addBindValue(existing_id);

        qDebug() << "deleting old tags" <<
                    sq.exec();


        sq.finish();

    }

    if(existing_id) {
        qDebug() << "update";

        sq.prepare("UPDATE BOT SET revision = ?, xml = ?, link = ?, icon = ?, description = ?, country = ? WHERE id = ?");
        sq.addBindValue( revision );
        sq.addBindValue( strXml );
        sq.addBindValue( link );
        sq.addBindValue( icon );
        sq.addBindValue( description );
        sq.addBindValue( country );
        sq.addBindValue( existing_id );


        qDebug() << "bot update " << sq.exec();

    } else {
        qDebug() << "insert";

        sq.prepare("INSERT INTO BOT (id, name, revision, enabled, xml, link, icon, description, country) VALUES(NULL, ?, ?, 1, ?, ?, ?, ?, ?);");
        sq.addBindValue( name );
        sq.addBindValue( revision );
        sq.addBindValue( strXml );
        sq.addBindValue( link );
        sq.addBindValue( icon );
        sq.addBindValue( description );
        sq.addBindValue( country );


        qDebug() << "bot insert " << sq.exec();

        existing_id = sq.lastInsertId().value<int>();

        m_botList.refresh();

        emit botList_changed(&m_botList);
        emit botList()->count_changed(botList()->rowCount());
    }


    // insert tags
    foreach(QString t, tagList) {
        t = t.trimmed();
        qDebug() << "inserting tag" << t;
        sq.prepare("INSERT INTO bot_tag (bot_id, tag) VALUES(?,?);");
        sq.addBindValue(existing_id);
        sq.addBindValue(t);
        sq.exec();
    }

    // insert parameters
    foreach(QVariantMap ip, insertBotParams) {

        qDebug() << "inserting param" << ip;

        sq.prepare("INSERT INTO bot_param (bot_id, key, type, title, value) "
                   "VALUES (?, ?, ?, ?, ?);");

        sq.addBindValue(existing_id);
        sq.addBindValue(ip.value("key") );
        sq.addBindValue(ip.value("type") );
        sq.addBindValue(ip.value("title") );        
        sq.addBindValue(ip.value("value", "") );

        qDebug() << existing_id <<
                    ip.value("key") <<
                    ip.value("title") <<
                    ip.value("value", "");

        qDebug() << "param insert" << sq.exec();

    }


    emit botDownloadSuccess(existing_id);

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

    qDebug() << "SELECT * FROM bot_param WHERE bot_id = " + QString::number(botId);

    emit paramList_changed(&m_paramList);
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

    QDBusMessage testcall =
    QDBusMessage::createMethodCall("com.omnight.phonehook",
                                   "/",
                                   "com.omnight.phonehook",
                                   "testLookup");

    QVariantList args;
    args.append(testNumber);
    args.append(botId);
    testcall.setArguments(args);

    QDBusConnection::sessionBus().send(testcall);

//    dbus_adapter::Instance()->clearReady();
//    dbus_adapter::Instance()->set_lookupState("");
//    lookup_thread *lt = new lookup_thread(testNumber, botId);
//    lt->start();
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

    qDebug() << "check" << name << rev;

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
    QSqlQuery qs("DELETE FROM BOT_COOKIE_CACHE WHERE bot_id = ?;");
    qs.addBindValue(botId);
    qs.exec();
}


QString bots::country() {
     return querySetting("location", "");
}
