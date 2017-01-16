#ifndef UTIL_H
#define UTIL_H

#include <QObject>
#include <QSqlQuery>
#include <QVariant>
#include <QVariantMap>
#include <QVariantList>

class util : public QObject
{
    Q_OBJECT

public:
    explicit util(QObject *parent = 0);

    static QVariant queryToArray(QString query, QVariantMap params, QSqlDatabase *db = NULL);

signals:

public slots:
};

#endif // UTIL_H
