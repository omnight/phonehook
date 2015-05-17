#ifndef BOTS_H
#define BOTS_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSql>
#include <QtSql/QSqlQuery>
#include <QDebug>
#include <QDirIterator>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDBusServiceWatcher>
#include <QDBusMessage>
#include "db_model.h"

class bots : public QObject
{
    Q_OBJECT

    Q_PROPERTY(PhSqlModel* botList READ botList NOTIFY botList_changed)
    Q_PROPERTY(PhSqlModel* botSearchList READ botSearchList NOTIFY botSearchList_changed)
    Q_PROPERTY(PhSqlModel* paramList READ paramList NOTIFY paramList_changed)
    Q_PROPERTY(bool daemonActive READ daemonActive NOTIFY daemonActive_changed)
    Q_PROPERTY(bool testSources READ testSources NOTIFY testSources_changed)
    Q_PROPERTY(QString country READ country NOTIFY country_changed)

public:
    explicit bots(QObject *parent = 0);

    ~bots();

    PhSqlModel *botList() {
        if(!m_botList.query().isValid())
            initBotList();

        return &m_botList;
    }
    PhSqlModel *botSearchList() { return &m_botSearchList; }
    PhSqlModel *paramList() { return &m_paramList; }
    bool daemonActive() { return m_daemonActive; }
    bool testSources() { return m_testSources; }
    QString country();

    Q_INVOKABLE QVariantMap getBotDetails(int botId);
    Q_INVOKABLE void downloadBot(QString file);
    Q_INVOKABLE void updateBotData(QVariantMap data);

    Q_INVOKABLE int getBotId(QString name);
    Q_INVOKABLE void setActiveBot(int botId);


    Q_INVOKABLE void initBotList();
    Q_INVOKABLE void setBotParam(int botId, QString key, QString value);
    Q_INVOKABLE void testBot(int botId, QString testNumber);
    Q_INVOKABLE bool removeBot(int botId);
    Q_INVOKABLE void startDaemon();
    Q_INVOKABLE int botStatusCompare(QString name, int rev);

    Q_INVOKABLE QString querySetting(QString key, QString def);
    Q_INVOKABLE void setSetting(QString key, QString value);

    Q_INVOKABLE QString getCountryName(QString code);
    Q_INVOKABLE int version();
    Q_INVOKABLE void clearCache(int botId);
    Q_INVOKABLE void setBotSearchListTag(QString tag);

signals:
    void botList_changed(PhSqlModel*);
    void botSearchList_changed(PhSqlModel*);
    void paramList_changed(PhSqlModel*);
    void daemonActive_changed(bool active);
    void botDownloadSuccess(int botId);
    void botDownloadFailure();
    void testSources_changed(bool testSources);
    void country_changed(QString country);

public slots:
    void network_response(QNetworkReply*);
    void service_registered(QString);
    void service_unregistered(QString);

private:
    QSqlDatabase m_db;
    PhSqlModel m_botList;
    PhSqlModel m_botSearchList;
    PhSqlModel m_paramList;
    int m_version;
    bool m_daemonActive;
    bool m_testSources;

    void initDbTables();
    QNetworkAccessManager netman;

    QVariantMap recordToVariantMap(QSqlRecord r);

    QDBusServiceWatcher serviceWatcher;

};

#endif // BOTS_H
