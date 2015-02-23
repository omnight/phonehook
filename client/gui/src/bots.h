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

class BotSqlModel : public QSqlQueryModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY count_changed)

public:


    BotSqlModel(QObject* parent = 0) : QSqlQueryModel(parent)
    {
    }

    QSqlQuery SQL_QUERY;

    void generateRoleNames()
    {
        roleNamesHash.clear();
        for( int i = 0; i < record().count(); i++) {
            roleNamesHash[Qt::UserRole + i + 1] = record().fieldName(i).toLocal8Bit();
        }
    }

    void setQuery(const QSqlQuery & query)
    {
        SQL_QUERY = query;
        QSqlQueryModel::setQuery(query);
        generateRoleNames();
        emit count_changed(rowCount());
    }

    QVariant data(const QModelIndex &index, int role) const
    {
        QVariant value = QSqlQueryModel::data(index, role);
        if(role < Qt::UserRole)
        {
            value = QSqlQueryModel::data(index, role);
        }
        else
        {
            int columnIdx = role - Qt::UserRole - 1;
            QModelIndex modelIndex = this->index(index.row(), columnIdx);
            value = QSqlQueryModel::data(modelIndex, Qt::DisplayRole);
        }
        return value;
    }



    void refresh() {
        QSqlQueryModel::clear();
        QSqlQueryModel::setQuery(SQL_QUERY.executedQuery());

        qDebug() << "setting query" << SQL_QUERY.lastQuery();

    }

    Q_INVOKABLE QString getValue(int row, QString column) {

        if(row >= rowCount())
            return "";

        for(QHash<int, QByteArray>::iterator r = roleNamesHash.begin(); r != roleNamesHash.end(); r++) {
            if(r.value().toLower() == column.toLower()) {
                return record(row).value(r.key() - Qt::UserRole - 1).toString();
            }
        }

        return "";
    }

    QHash<int, QByteArray> roleNames() const { return roleNamesHash; }

public:
    int count() {
        return rowCount();
    }

signals:
    void count_changed(int count);


private:
    QHash<int, QByteArray> roleNamesHash;
};

class bots : public QObject
{
    Q_OBJECT

    Q_PROPERTY(BotSqlModel* botList READ botList NOTIFY botList_changed)
    Q_PROPERTY(BotSqlModel* botSearchList READ botSearchList NOTIFY botSearchList_changed)
    Q_PROPERTY(BotSqlModel* paramList READ paramList NOTIFY paramList_changed)
    Q_PROPERTY(bool injectorActive READ injectorActive NOTIFY injectorActive_changed)
    Q_PROPERTY(bool daemonActive READ daemonActive NOTIFY daemonActive_changed)
    Q_PROPERTY(bool lipstickPatchInstalled READ lipstickPatchInstalled NOTIFY lipstickPatchInstalled_changed)
    Q_PROPERTY(bool testSources READ testSources NOTIFY testSources_changed)
    Q_PROPERTY(QString country READ country NOTIFY country_changed)

public:
    explicit bots(QObject *parent = 0);

    ~bots();

    BotSqlModel *botList() { return &m_botList; }
    BotSqlModel *botSearchList() { return &m_botSearchList; }
    BotSqlModel *paramList() { return &m_paramList; }
    bool injectorActive() { return m_injectorActive; }
    bool daemonActive() { return m_daemonActive; }
    bool lipstickPatchInstalled() { return m_lipstickPatched; }
    bool testSources() { return m_testSources; }
    QString country();

    Q_INVOKABLE QVariantMap getBotDetails(int botId);
    Q_INVOKABLE void downloadBot(QString file);
    Q_INVOKABLE void updateBotData(QVariantMap data);

    Q_INVOKABLE int getBotId(QString name);
    Q_INVOKABLE void setActiveBot(int botId);


    Q_INVOKABLE void setBotParam(int botId, QString key, QString value);
    Q_INVOKABLE void testBot(int botId, QString testNumber);
    Q_INVOKABLE bool removeBot(int botId);
    Q_INVOKABLE void restartSystem();
    Q_INVOKABLE void startDaemon();
    Q_INVOKABLE int botStatusCompare(QString name, int rev);

    Q_INVOKABLE QString querySetting(QString key, QString def);
    Q_INVOKABLE void setSetting(QString key, QString value);

    Q_INVOKABLE QString getCountryName(QString code);
    Q_INVOKABLE int version();
    Q_INVOKABLE void clearCache(int botId);
    Q_INVOKABLE void setBotSearchListTag(QString tag);

signals:
    void botList_changed(BotSqlModel*);
    void botSearchList_changed(BotSqlModel*);
    void paramList_changed(BotSqlModel*);
    void injectorActive_changed(bool active);
    void daemonActive_changed(bool active);
    void botDownloadSuccess(int botId);
    void botDownloadFailure();
    void lipstickPatchInstalled_changed(bool installed);
    void testSources_changed(bool testSources);
    void country_changed(QString country);

public slots:
    void network_response(QNetworkReply*);
    void service_registered(QString);
    void service_unregistered(QString);
    void cs_introspect(QDBusMessage m);

private:
    QSqlDatabase m_db;
    BotSqlModel m_botList;
    BotSqlModel m_botSearchList;
    BotSqlModel m_paramList;
    int m_version;
    bool m_injectorActive;
    bool m_daemonActive;
    bool m_lipstickPatched;
    bool m_testSources;

    void initDbTables();
    QNetworkAccessManager netman;

    QVariantMap recordToVariantMap(QSqlRecord r);

    QDBusServiceWatcher serviceWatcher;

};

#endif // BOTS_H
