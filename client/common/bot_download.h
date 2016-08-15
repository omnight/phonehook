#ifndef bot_download_H
#define bot_download_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSql>
#include <QtSql/QSqlQuery>
#include <QDebug>
#include <QDirIterator>
#include <QSqlQueryModel>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>



class bot_download : public QObject
{
    Q_OBJECT

public:
    explicit bot_download(QObject *parent = 0);

    Q_INVOKABLE void download(QString file, bool KeepData);

signals:
    void botDownloadSuccess(int botId);
    void botDownloadFailure();

public slots:
    void network_response(QNetworkReply*);

private:

    QNetworkAccessManager netman;

};

#endif // BOTS_H
