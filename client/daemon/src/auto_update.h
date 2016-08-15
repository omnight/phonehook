#ifndef AUTO_UPDATE_H
#define AUTO_UPDATE_H

#include <QObject>
#include <QTimer>
#include <QNetworkAccessManager>

#include "macros.h"
#include "setting.h"
#include "bot_download.h"

class auto_update : public QObject
{
    Q_OBJECT
    SINGLETON(auto_update)

private:
    QTimer updateTimer;
    QNetworkAccessManager netMan;
    bot_download *botMan;
    QStringList botDownloadQueue;

public:
    explicit auto_update(QObject *parent = 0);    

    Q_INVOKABLE void scheduleNext();

signals:

public slots:
    void checkForUpdates();
    void gotServerResponse(QNetworkReply *reply);

    void botDownloadFinish(int botId);
    void botDownloadFailure();

};

#endif // AUTO_UPDATE_H
