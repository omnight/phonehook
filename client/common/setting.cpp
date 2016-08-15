#include "setting.h"
#include <QSqlQuery>
#include <QVariant>

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
    QSqlQuery sq;
    sq.prepare("INSERT OR REPLACE INTO setting (key, value) "
               "VALUES (?,?)");

    sq.addBindValue(key);
    sq.addBindValue(value);
    sq.exec();
}
