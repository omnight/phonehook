#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#include "auto_update.h"

SINGLETON_CPP(auto_update)

auto_update::auto_update(QObject *parent) : QObject(parent)
{

    connect(&updateTimer, &QTimer::timeout, this, &auto_update::checkForUpdates);
    connect(&netMan, &QNetworkAccessManager::finished, this, &auto_update::gotServerResponse);

    botMan = NULL;

    //updateTimer.setInterval(1000);
    updateTimer.setSingleShot(true);
    scheduleNext();

}

void auto_update::scheduleNext() {
    bool isEnabled = (setting::get("auto_update_enabled", "true") == "true");

    if(!isEnabled) {
        updateTimer.start(60000);
        return;
    }

    QString lastCheck = setting::get("auto_update_last_check", "");

    if(lastCheck == "") {
        checkForUpdates();
    } else {
        QDateTime lastCheckDate = QDateTime::fromString(lastCheck, Qt::ISODate);

        qint64 timeToNext =
            QDateTime::currentDateTimeUtc().msecsTo(lastCheckDate.addDays(1));

        if(timeToNext < 0) {
            checkForUpdates();
        } else {
            updateTimer.stop();
            updateTimer.start(timeToNext);

            qDebug() << "next update scheduled in" << timeToNext << "msecs";
        }

    }

}

void auto_update::checkForUpdates() {

    bool isEnabled = (setting::get("auto_update_enabled", "true") == "true");

    if(!isEnabled) {
        updateTimer.start(60000);
        return;
    }

    qDebug() << "starting source update!";

    QNetworkRequest req;
    QString autoUpdateIndexUrl = setting::get("auto_update_index_url", "https://raw.githubusercontent.com/omnight/phonehook-sources/master/files/i.js");

    req.setUrl(QUrl(autoUpdateIndexUrl));

    netMan.get(req);

}

void auto_update::gotServerResponse(QNetworkReply *reply) {

    if(reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt() == 200) {
        qDebug() << "got update list";

        
        QVariantList installedBots =
        SQL("SELECT name, revision FROM bot").value<QVariantList>();
        
        auto response = reply->readAll();
        QVariantList serverBots = QJsonDocument::fromJson(response).array().toVariantList();


        botDownloadQueue.clear();

        foreach(QVariant ib, installedBots) {

            QString botName = ib.value<QVariantMap>()["name"].toString();
            int revision = ib.value<QVariantMap>()["revision"].toInt();


            foreach(QVariant b, serverBots) {
                /*qDebug() << b.value<QVariantMap>()["n"].toString()
                        << botName
                        << b.value<QVariantMap>()["v"].toInt()
                        << revision;*/

                if(b.value<QVariantMap>()["n"].toString() == botName && b.value<QVariantMap>()["v"].toInt() > revision) {
                    qDebug() << "add" << b.value<QVariantMap>()["f"].toString();
                    botDownloadQueue << b.value<QVariantMap>()["f"].toString();
                }
            }
        }

        if(botDownloadQueue.count() > 0) {
            botMan = new bot_download();

            connect(botMan, &bot_download::botDownloadSuccess, this, &auto_update::botDownloadFinish);
            connect(botMan, &bot_download::botDownloadFailure, this, &auto_update::botDownloadFailure);

            botMan->download(botDownloadQueue.first(), true);
        } else {
            setting::put("auto_update_last_check", QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
            scheduleNext();
        }
    }

    reply->deleteLater();
}

void auto_update::botDownloadFailure() {
    // abort
    botMan->deleteLater();  
    setting::put("auto_update_last_check", QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    scheduleNext();
}


void auto_update::botDownloadFinish(int botId) {

    qDebug() << "bot updated" << botId;

    botDownloadQueue.pop_front();

    if(botDownloadQueue.count() > 0) {
        botMan->download(botDownloadQueue.first(), true);
    } else {
        qDebug() << "no more in list";
        botMan->deleteLater();
        botMan = NULL;
        setting::put("auto_update_last_check", QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
        scheduleNext();
    }

}
