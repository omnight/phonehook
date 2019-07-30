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
#include <QNetworkCookie>
#include <QNetworkCookieJar>

#include <QQuickView>
#include <QQmlEngine>

#include <private/qquickwebview_p.h>
#include "bot_download.h"

class OpenJar: public QNetworkCookieJar {
    Q_OBJECT
public:
    explicit OpenJar(QObject *parent=0): QNetworkCookieJar(parent) { }
    QList<QNetworkCookie> allCookies() {
        return QNetworkCookieJar::allCookies();
    }

};

class bots : public QObject
{
    Q_OBJECT

    Q_PROPERTY(PhSqlModel* botList READ botList NOTIFY botList_changed)
    Q_PROPERTY(PhSqlModel* botSearchList READ botSearchList NOTIFY botSearchList_changed)
    Q_PROPERTY(PhSqlModel* paramList READ paramList NOTIFY paramList_changed)
    Q_PROPERTY(PhSqlModel* loginList READ loginList NOTIFY loginList_changed)

    Q_PROPERTY(bool daemonActive READ daemonActive NOTIFY daemonActive_changed)
    Q_PROPERTY(bool testSources READ testSources NOTIFY testSources_changed)

private:
    bot_download botDownloader;

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
    PhSqlModel *loginList() { return &m_loginList; }

    bool daemonActive() { return m_daemonActive; }
    bool testSources() { return m_testSources; }

    Q_INVOKABLE QVariantMap getBotDetails(int botId);

    Q_INVOKABLE int getBotId(const QString &name);
    Q_INVOKABLE void setActiveBot(int botId);
    Q_INVOKABLE void downloadBot(const QString& file, bool KeepData);

    Q_INVOKABLE void initBotList();
    Q_INVOKABLE void setBotParam(int botId, const QString &key, const QString &value);
    Q_INVOKABLE void testBot(int botId, const QString &testNumber);
    Q_INVOKABLE bool removeBot(int botId);
    Q_INVOKABLE void startDaemon();
    Q_INVOKABLE int botStatusCompare(const QString &name, int rev);

    Q_INVOKABLE QString getCountryName(const QString &code);
    Q_INVOKABLE int version();
    Q_INVOKABLE void clearCache(int botId);
    Q_INVOKABLE void setBotSearchListTag(const QString &tag);

    Q_INVOKABLE bool hasBlockTag(int botId);
    Q_INVOKABLE bool isBlockSource(int botId);
    Q_INVOKABLE void setBlockSource(int botId, bool enabled);

    OpenJar jar;

    Q_INVOKABLE int cookieCount()
    {
        return jar.allCookies().length();
    }

    Q_INVOKABLE void setCookieManager(QQuickWebView *view) {
        //view->experimental()->deleteAllCookies();
        //qDebug() << view->experimental()->dynamicPropertyNames();
        //view->experimental()->evaluateJavaScript();
        //view->engine()->networkAccessManager()->setCookieJar(&jar);
    }

    Q_INVOKABLE void copyCookies(int bot_id);


    Q_INVOKABLE void vCardWrite(const QString &name, const QStringList &numbers, QString address);

signals:
    void botList_changed(PhSqlModel*);
    void botSearchList_changed(PhSqlModel*);
    void paramList_changed(PhSqlModel*);
    void loginList_changed(PhSqlModel*);

    void daemonActive_changed(bool active);
    void botDownloadSuccess(int botId);
    void botDownloadFailure();
    void testSources_changed(bool testSources);
    void country_changed(QString country);

public slots:

    void botDownload_finish(int botId);
    void botDownload_fail();
    void service_registered(const QString&);
    void service_unregistered(const QString&);

private:
    QSqlDatabase m_db;
    PhSqlModel m_botList;
    PhSqlModel m_botSearchList;
    PhSqlModel m_paramList;
    PhSqlModel m_loginList;
    int m_version;
    bool m_daemonActive;
    bool m_testSources;

    void initDbTables();

    QVariantMap recordToVariantMap(const QSqlRecord &r);

    QDBusServiceWatcher serviceWatcher;

};

#endif // BOTS_H
