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

#include "bot_download.h"
#include "setting.h"

bot_download::bot_download(QObject *parent) :
    QObject(parent)
{
    connect(&netman, &QNetworkAccessManager::finished , this, &bot_download::network_response);
}


void bot_download::download(QString file, bool KeepData) {
    QNetworkRequest req;
    QString downloadUrlRoot = setting::get("download_root_url", "https://raw.githubusercontent.com/omnight/phonehook-sources/master/files/");

    req.setUrl(QUrl(downloadUrlRoot + file));
    req.setRawHeader("KeepData", KeepData ? "true" : "false");
    QNetworkReply* rep = netman.get(req);
    Q_UNUSED(rep)
}




void bot_download::network_response(QNetworkReply *reply) {

    QXmlQuery xq;
    QSqlQuery sq;

    bool KeepData = reply->request().rawHeader("KeepData") == "true" ? true : false;

    if(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute) != 200) {
        emit botDownloadFailure();
        return;
    }

    QString strXml = reply->readAll();

    if(strXml == "") {
        emit botDownloadFailure();
        return;
    }

    xq.setFocus(strXml);

    QString name, revision, description, icon, link, country;
    xq.setQuery("robot/meta/name/string()");
    xq.evaluateTo(&name);
    name = name.trimmed().replace("&amp;","&");

    xq.setQuery("robot/meta/revision/number()");
    xq.evaluateTo(&revision);

    xq.setQuery("robot/meta/description/string()");
    xq.evaluateTo(&description);
    description = description.trimmed().replace("&amp;","&");;

    xq.setQuery("robot/meta/country/string()");
    xq.evaluateTo(&country);
    country = country.trimmed();

    xq.setQuery("robot/meta/icon/string()");
    xq.evaluateTo(&icon);
    icon = icon.trimmed();

    xq.setQuery("robot/meta/link/string()");
    xq.evaluateTo(&link);
    link = link.trimmed().replace("&amp;","&");;

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
        //emit botList_changed(&m_botList);
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

        //m_botList.refresh();

        //emit botList_changed(&m_botList);
        //emit botList()->count_changed(botList()->rowCount());
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

    // insert login methods

    QXmlResultItems logins;
    xq.setFocus(strXml);
    xq.setQuery("robot/meta/login");
    xq.evaluateTo(&logins);

    sq.prepare("DELETE FROM bot_login WHERE bot_id=?;");
    sq.addBindValue(existing_id);
    sq.exec();

    while(!logins.next().isNull()) {
        sq.prepare(R"(
            INSERT INTO bot_login (bot_id,name,url,login_success_url,login_success_tag)
            VALUES(?,?,?,?,?);
        )");
        sq.addBindValue(existing_id);

        QString v;
        xq.setFocus( logins.current() );
        xq.setQuery("./name/string()");
        xq.evaluateTo(&v);
        sq.addBindValue(v.trimmed().replace("&amp;","&"));

        xq.setFocus( logins.current() );
        xq.setQuery("./url/string()");
        xq.evaluateTo(&v);
        sq.addBindValue(v.trimmed().replace("&amp;","&"));

        xq.setFocus( logins.current() );
        xq.setQuery("./login_success_url/string()");
        xq.evaluateTo(&v);
        sq.addBindValue(v.trimmed().replace("&amp;","&"));

        xq.setFocus( logins.current() );
        xq.setQuery("./login_success_tag/string()");
        xq.evaluateTo(&v);
        sq.addBindValue(v.trimmed());

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

    if(!KeepData) {
        sq.prepare("DELETE FROM bot_response_cache WHERE bot_id=?");
        sq.addBindValue(existing_id);
        qDebug() << "cleared response cache" << sq.exec();

        sq.prepare("DELETE FROM bot_result_cache WHERE bot_id=?");
        sq.addBindValue(existing_id);
        qDebug() << "cleared result cache" << sq.exec();
    }

    emit botDownloadSuccess(existing_id);

}
