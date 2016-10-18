#ifndef SETTING_H
#define SETTING_H

#include <QObject>
#include "macros.h"

class setting : public QObject
{
    Q_OBJECT

    SINGLETON(setting)

public:
    explicit setting(QObject *parent = 0);

    Q_INVOKABLE static QString get(QString key, QString defValue);
    Q_INVOKABLE static void put(QString key, QString value);
    Q_INVOKABLE static void remove(QString key);

signals:

public slots:

};

#endif // SETTING_H
