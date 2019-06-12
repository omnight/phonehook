#include <QSqlQuery>
#include <QVariant>

#include "setting.h"

SINGLETON_CPP(setting)

setting::setting(QObject *parent) :
    QObject(parent)
{
}


QString setting::get(QString key, QString defValue) {

    QSqlQuery ptq;
    ptq.prepare("SELECT value FROM setting WHERE key = ?");
    ptq.addBindValue(key);
    ptq.exec();


    if(ptq.next()) {
        return ptq.value("value").toString();
    }

    return defValue;
}

void setting::put(QString key, QString value) {
    SQL("\
        INSERT OR REPLACE INTO setting (key, value)\
        VALUES (:key, :value)\
    ", key, value);
}

void setting::remove(QString key) {
    SQL("\
        DELETE FROM setting WHERE key = :key\
    ", key);
}
